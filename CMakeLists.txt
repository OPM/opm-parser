cmake_minimum_required(VERSION 2.8.12)
project(opm-parser CXX)

file(STRINGS dune.module dm)
if ("${dm}" MATCHES "Version: ([a-zA-Z0-9.-]*);")
    string(REGEX MATCHALL "[a-z0-9]+" vers ${CMAKE_MATCH_1})
    list(GET vers 0 opm-parser_VERSION_MAJOR)
    list(GET vers 1 opm-parser_VERSION_MINOR)

    message(STATUS "opm-parser version set to "
                   ${opm-parser_VERSION_MAJOR} "."
                   ${opm-parser_VERSION_MINOR} " in dune.module")
else ()
    message(SEND_ERROR "Could not read version from dune.module")
endif ()

include(CTest)
include(GNUInstallDirs)
include(TestCXXAcceptsFlag)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type specified - defaulting to 'Release'.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose build type." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY
                 STRINGS "Release" "Debug" "RelWithDebInfo" "MinSizeRel")
endif()

option(BUILD_TESTING "Build test applications by default?"          ON)
option(USE_RUNPATH   "Embed dependency paths in installed library"  ON)
option(SIBLING_SEARCH "Search for other modules in sibling directories?" ON)

#-----------------------------------------------------------------

if (USE_RUNPATH)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif ()

if (MSVC)
    add_definitions( "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4996 /wd4244 /wd4267")
    set(CMAKE_CXX_FLAGS "/MP ${CMAKE_CXX_FLAGS}")
else()
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    list(APPEND debug-flags -pedantic -Wall -Wextra -Wformat-nonliteral
                            -Wcast-align -Wpointer-arith -Wmissing-declarations
                            -Wcast-qual -Wshadow -Wwrite-strings
                            -Wchar-subscripts -Wredundant-decls)
    string(REGEX REPLACE ";" " " debug-flags "${debug-flags}")
    set(CMAKE_CXX_FLAGS_DEBUG "${debug-flags} ${CMAKE_CXX_FLAGS_DEBUG}")
endif()

#-----------------------------------------------------------------
if(SIBLING_SEARCH AND NOT ecl_DIR)
  # guess the sibling dir
  get_filename_component(_leaf_dir_name ${PROJECT_BINARY_DIR} NAME)
  get_filename_component(_parent_full_dir ${PROJECT_BINARY_DIR} DIRECTORY)
  get_filename_component(_parent_dir_name ${_parent_full_dir} NAME)
  #Try if <module-name>/<build-dir> is used
  get_filename_component(_modules_dir ${_parent_full_dir} DIRECTORY)
  if(IS_DIRECTORY ${_modules_dir}/libecl/${_leaf_dir_name})
    set(ecl_DIR ${_modules_dir}/libecl/${_leaf_dir_name})
  else()
    string(REPLACE ${PROJECT_NAME} libecl _opm_common_leaf ${_leaf_dir_name})
    if(NOT _leaf_dir_name STREQUAL _opm_common_leaf
        AND IS_DIRECTORY ${_parent_full_dir}/${_opm_common_leaf})
      # We are using build directories named <prefix><module-name><postfix>
      set(ecl_DIR ${_parent_full_dir}/${_opm_common_leaf})
    elseif(IS_DIRECTORY ${_parent_full_dir}/libecl)
      # All modules are in a common build dir
      set(ecl_DIR "${_parent_full_dir}/libecl}")
    endif()
  endif()
endif()
if(ecl_DIR AND NOT IS_DIRECTORY ${ecl_DIR})
  message(WARNING "Value ${ecl_DIR} passed to variable"
    " ecl_DIR is not a directory")
endif()

# Give us a chance to find ecl installed to CMAKE_INSTALL_PREFIX.
# We need to deactivate the package registry for this.
find_package(ecl NO_CMAKE_SYSTEM_PACKAGE_REGISTRY NO_CMAKE_PACKAGE_REGISTRY)
# If everything else failed fall back to the registry
find_package(ecl REQUIRED)
message(STATUS "Found ecl library: ${ecl_DIR}")
find_library(CJSON_LIBRARY NAMES cjson)
if (CJSON_LIBRARY)
    message(STATUS "Found CJSON library: ${CJSON_LIBRARY}")
    add_library(cjson UNKNOWN IMPORTED)
    set_target_properties(cjson PROPERTIES IMPORTED_LOCATION ${CJSON_LIBRARY})
else ()
    add_subdirectory(external/cjson)
endif()

# if building shared libraries, then don't include Boost in them
if (BUILD_SHARED_LIBS)
    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_STATIC_RUNTIME OFF)
else ()
# if using dynamic boost, the header file must generate a main() function
   set(Boost_USE_STATIC_LIBS ON)
endif ()

find_package(Boost 1.44.0
             COMPONENTS filesystem
                        date_time
                        system
                        unit_test_framework
                        regex
             REQUIRED)

# boost libraries are often named with -mt, -d, -g etc. when they're configured
# in a particular way, and should be linked to precisely these libraries.
# create a target name from a found boost lib, possibly adjusted to the build
# type.
function(boost_target target component)
    function(libname var lib)
        get_filename_component(name "${lib}" NAME_WE)
        string(SUBSTRING ${name} 3 -1 name)
        set(${var} ${name} PARENT_SCOPE)
    endfunction()

    unset(lib_name)
    list(LENGTH ARGN library_list_length)
    # list only contains one element (the library itself) - strip the lib
    # prefix and return
    if (library_list_length EQUAL 1)
        libname(lib_name ${ARGN})
        set(${target} ${lib_name} PARENT_SCOPE)
        return()
    endif()

    # Build type is one of the standard (debug, release, relwithdebinfo,
    # minsizerel and we choose build/release accordingly
    string(TOUPPER "${CMAKE_BUILD_TYPE}" _build_type)
    set(lib_name ${Boost_${component}_LIBRARY})

    if ("${_build_type}" STREQUAL "DEBUG")
        if (Boost_${component}_LIBRARY_DEBUG)
            set(lib_name ${Boost_${component}_LIBRARY_DEBUG})
        endif()

        libname(lib_name ${lib_name})
        set(${target} ${lib_name} PARENT_SCOPE)
        return ()
    endif()

    if ("${_build_type}" MATCHES "(RELEASE)|(RELWITHDEBINFO)|(MINSIZEREL)")
        if (Boost_${component}_LIBRARY_RELEASE)
            set(lib_name ${Boost_${component}_LIBRARY_RELEASE})
        endif()

        libname(lib_name ${lib_name})
        set(${target} ${lib_name} PARENT_SCOPE)
        return ()
    endif()

    # check if it is some custom build type that's also a debug type
    # if not, assume linking to optimized is what we want
    list(FIND DEBUG_CONFIGURATIONS "${CMAKE_BUILD_TYPE}" _is_debug)
    if (NOT _is_debug EQUAL -1)
        set(key debug)
    else()
        set(key optimized)
    endif ()

    list(FIND ARGN ${key} _index)
    math(EXPR _index "${_index} + 1")
    list(GET ARGN ${_index} lib_name)

    libname(lib_name ${lib_name})
    set(${target} ${lib_name} PARENT_SCOPE)
endfunction()

# make targets for boost
boost_target(boost_system SYSTEM ${Boost_SYSTEM_LIBRARY})
add_library(${boost_system} UNKNOWN IMPORTED)
set_target_properties(${boost_system} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
    IMPORTED_LOCATION         "${Boost_SYSTEM_LIBRARY}"
    IMPORTED_LOCATION_DEBUG   "${Boost_SYSTEM_LIBRARY_DEBUG}"
    IMPORTED_LOCATION_RELEASE "${Boost_SYSTEM_LIBRARY_RELEASE}"
)

boost_target( boost_filesystem FILESYSTEM ${Boost_FILESYSTEM_LIBRARY} )
add_library(${boost_filesystem} UNKNOWN IMPORTED)
set_target_properties(${boost_filesystem} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
    INTERFACE_COMPILE_DEFINITIONS BOOST_FILESYSTEM_VERSION=3
    INTERFACE_LINK_LIBRARIES  "${boost_system}"
    IMPORTED_LOCATION         "${Boost_FILESYSTEM_LIBRARY}"
    IMPORTED_LOCATION_DEBUG   "${Boost_FILESYSTEM_LIBRARY_DEBUG}"
    IMPORTED_LOCATION_RELEASE "${Boost_FILESYSTEM_LIBRARY_RELEASE}"
)

boost_target( boost_date_time DATE_TIME ${Boost_DATE_TIME_LIBRARY} )
add_library(${boost_date_time} UNKNOWN IMPORTED)
set_target_properties(${boost_date_time} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
    IMPORTED_LOCATION         "${Boost_DATE_TIME_LIBRARY}"
    IMPORTED_LOCATION_DEBUG   "${Boost_DATE_TIME_LIBRARY_DEBUG}"
    IMPORTED_LOCATION_RELEASE "${Boost_DATE_TIME_LIBRARY_RELEASE}"
)


boost_target( boost_regex REGEX ${Boost_REGEX_LIBRARY} )
add_library(${boost_regex} UNKNOWN IMPORTED)
set_target_properties(${boost_regex} PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES  "${boost_system}"
    IMPORTED_LOCATION         "${Boost_REGEX_LIBRARY}"
    IMPORTED_LOCATION_DEBUG   "${Boost_REGEX_LIBRARY_DEBUG}"
    IMPORTED_LOCATION_RELEASE "${Boost_REGEX_LIBRARY_RELEASE}"
)

add_library(boost_test UNKNOWN IMPORTED)
set_target_properties(boost_test PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${Boost_INCLUDE_DIRS}"
    INTERFACE_LINK_LIBRARIES  "${boost_system}"
    IMPORTED_LOCATION         "${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}"
    IMPORTED_LOCATION_DEBUG   "${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_DEBUG}"
    IMPORTED_LOCATION_RELEASE "${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_RELEASE}"
)

if (BUILD_SHARED_LIBS)
    set_target_properties(boost_test PROPERTIES
        INTERFACE_COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
    )
endif ()

#-----------------------------------------------------------------

include( cmake/Modules/CheckCaseSensitiveFileSystem.cmake )

get_filename_component(OPM_DATA_ROOT ${PROJECT_SOURCE_DIR}/../opm-data
                       ABSOLUTE CACHE)

if (EXISTS ${OPM_DATA_ROOT}/norne/NORNE_ATW2013.DATA)
    set(HAVE_OPM_DATA TRUE)
else()
    set(HAVE_OPM_DATA FALSE)
    message(WARNING "opm-data not found - skipping some integration tests.")
endif ()

# Setup static analysis
include( cmake/Modules/UseStaticAnalysis.cmake )
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules)
setup_static_analysis_tools()

add_subdirectory(lib/json)
add_subdirectory(lib/eclipse)
add_subdirectory(applications)

set(OPM_PARSER_PREFIX "${CMAKE_INSTALL_PREFIX}")
set(OPM_PARSER_MODULE_PATH "${OPM_PARSER_PREFIX}/share/dune/cmake/modules")
set(OPM_PARSER_INCLUDEDIRS "${OPM_PARSER_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
set(OPM_PARSER_LIBDIRS "${OPM_PARSER_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
set(OPM_PARSER_INSTALLED ON)
get_target_property(ecl_lib ecl LOCATION)
get_target_property(libtype opmparser TYPE)
if("${libtype}" STREQUAL "STATIC_LIBRARY")
  set(suffix ${CMAKE_STATIC_LIBRARY_SUFFIX})
  set(prefix ${CMAKE_STATIC_LIBRARY_PREFIX})
else()
  set(suffix ${CMAKE_SHARED_LIBRARY_SUFFIX})
  set(prefix ${CMAKE_SHARED_LIBRARY_PREFIX})
endif()
set(OPM_PARSER_LIBRARIES opmparser opmjson ${ecl_lib} ${Boost_FILESYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY})
set(OPM_PARSER_LIBRARY ${CMAKE_INSTALL_PREFIX}/lib/${prefix}opmparser${suffix})
set(ECL_DIR "${OPM_PARSER_PREFIX}/share/cmake/ecl")
configure_file(opm-parser-config.cmake.in
  "${PROJECT_BINARY_DIR}/install/opm-parser-config.cmake" @ONLY)

set(OPM_PARSER_PREFIX "${PROJECT_SOURCE_DIR}")
set(OPM_PARSER_INCLUDEDIRS "${OPM_PARSER_PREFIX}/lib/eclipse/include;${OPM_PARSER_PREFIX}/lib/json/include;${PROJECT_BINARY_DIR}/lib/eclipse/include")
set(OPM_PARSER_MODULE_PATH "${OPM_PARSER_PREFIX}/cmake/modules")
set(OPM_PARSER_LIBDIRS "${CMAKE_BINARY_DIR}/lib/eclipse;${CMAKE_BINARY_DIR}/lib/json")
set(OPM_PARSER_LIBRARY ${CMAKE_BINARY_DIR}/lib/eclipse/${prefix}opmparser${suffix})
set(OPM_PARSER_INSTALLED OFF)
set(ECL_DIR "${ecl_DIR}")
configure_file(opm-parser-config.cmake.in
  "${PROJECT_BINARY_DIR}/opm-parser-config.cmake" @ONLY)

configure_file(opm-parser-config-version.cmake.in
  "${PROJECT_BINARY_DIR}/opm-parser-config-version.cmake" @ONLY)

configure_file(opm-parser.pc.in
  "${PROJECT_BINARY_DIR}/opm-parser.pc" @ONLY)

install(FILES "dune.module" DESTINATION "lib/dunecontrol/opm-parser")
install(FILES  "config.h.cmake" DESTINATION "share/opm-parser")
install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/opm-parser.pc" DESTINATION "lib/pkgconfig")
install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/install/opm-parser-config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/opm-parser-config-version.cmake" DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/opm-parser")
export(TARGETS opmjson opmparser FILE opm-parser-targets.cmake)
install(EXPORT opm-parser-targets DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/opm-parser")
