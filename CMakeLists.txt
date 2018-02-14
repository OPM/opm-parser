###########################################################################
#                                                                         #
# Note: The bulk of the build system is located in opm-common.            #
#       This file only contains the specializations for this particular   #
#       project. Most likely you are interested in editing one of these   #
#       files instead:                                                    #
#                                                                         #
#       dune.module                              Name and version number  #
#       CMakeLists_files.cmake                   Path of source files     #
#       opm-parser-prereqs.cmake                 Dependencies             #
#                                                                         #
###########################################################################

# Mandatory call to project
project(opm-parser CXX)

cmake_minimum_required(VERSION 2.8)

option(SIBLING_SEARCH "Search for other modules in sibling directories?" ON)

if(SIBLING_SEARCH AND NOT opm-common_DIR)
  # guess the sibling dir
  get_filename_component(_leaf_dir_name ${PROJECT_BINARY_DIR} NAME)
  get_filename_component(_parent_full_dir ${PROJECT_BINARY_DIR} DIRECTORY)
  get_filename_component(_parent_dir_name ${_parent_full_dir} NAME)
  #Try if <module-name>/<build-dir> is used
  get_filename_component(_modules_dir ${_parent_full_dir} DIRECTORY)
  if(IS_DIRECTORY ${_modules_dir}/opm-common/${_leaf_dir_name})
    set(opm-common_DIR ${_modules_dir}/opm-common/${_leaf_dir_name})
  else()
    string(REPLACE ${PROJECT_NAME} opm-common _opm_common_leaf ${_leaf_dir_name})
    if(NOT _leaf_dir_name STREQUAL _opm_common_leaf
        AND IS_DIRECTORY ${_parent_full_dir}/${_opm_common_leaf})
      # We are using build directories named <prefix><module-name><postfix>
      set(opm-common_DIR ${_parent_full_dir}/${_opm_common_leaf})
    elseif(IS_DIRECTORY ${_parent_full_dir}/opm-common)
      # All modules are in a common build dir
      set(opm-common_DIR "${_parent_full_dir}/opm-common")
    endif()
  endif()
endif()
if(opm-common_DIR AND NOT IS_DIRECTORY ${opm-common_DIR})
  message(WARNING "Value ${opm-common_DIR} passed to variable"
    " opm-common_DIR is not a directory")
endif()

# if building shared libraries, then don't include Boost in them
if (BUILD_SHARED_LIBS)
    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_STATIC_RUNTIME OFF)
else ()
# if using dynamic boost, the header file must generate a main() function
   set(Boost_USE_STATIC_LIBS ON)
endif ()

find_package(opm-common REQUIRED)
include(OpmInit)

# Look for the opm-data repository; if found the variable
# HAVE_OPM_DATA will be set to true.
include(Findopm-data)

# not the same location as most of the other projects; this hook overrides
macro (dir_hook)
  set (doxy_dir "docs")
endmacro (dir_hook)

# list of prerequisites for this particular project; this is in a
# separate file (in cmake/Modules sub-directory) because it is shared
# with the find module
include (${project}-prereqs)

# read the list of components from this file (in the project directory);
# it should set various lists with the names of the files to include
include (CMakeLists_files.cmake)

macro (config_hook)
  set(EXTRA_INCLUDES ${PROJECT_SOURCE_DIR}/lib/eclipse/include
                     ${PROJECT_SOURCE_DIR}/lib/json/include)
  if(NOT cjson_FOUND)
    list(APPEND EXTRA_INCLUDES ${PROJECT_SOURCE_DIR}/external/cjson)
  endif()
  # For this project
  include_directories(${EXTRA_INCLUDES})

  # For downstreams
  list(APPEND EXTRA_INCLUDES ${PROJECT_BINARY_DIR}/include)
  set(OPM_PROJECT_EXTRA_CODE_INTREE "list(APPEND opm-parser_INCLUDE_DIRS ${EXTRA_INCLUDES})")

  # Configure boost targets for old cmake
  include(cmake/Modules/BoostTargets.cmake)

  if (BUILD_SHARED_LIBS)
    set_target_properties(Boost::unit_test_framework PROPERTIES INTERFACE_COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK=1)
  endif()
endmacro (config_hook)

macro (prereqs_hook)
endmacro (prereqs_hook)

macro (sources_hook)
  # Add json library
  set(opmjson_SOURCES lib/json/JsonObject.cpp)
  if(NOT cjson_FOUND)
    list(APPEND opmjson_SOURCES ${PROJECT_SOURCE_DIR}/external/cjson/cJSON.c)
  endif()
  add_library(opmjson ${opmjson_SOURCES})
  target_link_libraries(opmjson PUBLIC ${Boost_FILESYSTEM_LIBRARY})
  if(cjson_FOUND)
    target_link_libraries(opmjson PUBLIC ${CJSON_LIBRARIES})
  else()
    target_include_directories(opmjson PRIVATE ${PROJECT_SOURCE_DIR}/external/cjson)
  endif()
  list(APPEND opm-parser_LIBRARIES opmjson ecl)

  # Keyword generation
  include(GenerateKeywords.cmake)

  # Append generated sources
  list(APPEND opm-parser_SOURCES ${PROJECT_BINARY_DIR}/ParserKeywords.cpp)
endmacro (sources_hook)

macro (fortran_hook)
endmacro (fortran_hook)

macro (files_hook)
endmacro (files_hook)

macro (tests_hook)
  include(ExtraTests.cmake)
endmacro (tests_hook)

macro (install_hook)
  install(DIRECTORY lib/eclipse/include/ DESTINATION include)
  install(DIRECTORY ${PROJECT_BINARY_DIR}/include/
          DESTINATION include
          PATTERN *.hpp
  )
  install(DIRECTORY lib/json/include/ DESTINATION include)
  set(opm-parser_EXTRA_TARGETS opmjson)
endmacro (install_hook)

include (OpmLibMain)

# Add the build tree include directory as private relevant targets
list(APPEND EXTRA_TESTS opmparser opmi)
foreach(TARGET ${tests_SOURCES})
  get_filename_component (_sat_NAME "${TARGET}" NAME_WE)
  list(APPEND EXTRA_TESTS ${_sat_NAME})
endforeach()

foreach(TARGET ${EXTRA_TESTS})
  target_include_directories(${TARGET} PRIVATE ${PROJECT_BINARY_DIR}/include)
endforeach()
