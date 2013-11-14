cmake_minimum_required (VERSION 2.6)
project (OPMParser CXX)
enable_testing()
enable_language(C)


SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
SET( CMAKE_CXX_FLAGS "-pipe -Wall -Wno-unknown-pragmas -std=c++0x")
SET( CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -O0 -DDEBUG  -ggdb3")
SET( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG -mtune=native")

SET( CMAKE_C_FLAGS "-pipe -Wall -Wno-unknown-pragmas -std=c99")
SET( CMAKE_C_FLAGS_DEBUG   "${CMAKE_C_FLAGS_DEBUG}   -O0 -DDEBUG -ggdb3")
SET( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG -mtune=native")

option( INCLUDE_STATOIL_TESTS  "Include the tests which need proprietary Statoil data" OFF)

if (BOOST_ROOT AND NOT DEFINED Boost_NO_SYSTEM_PATHS)
   set (Boost_NO_SYSTEM_PATHS TRUE)
endif (BOOST_ROOT AND NOT DEFINED Boost_NO_SYSTEM_PATHS)

# if we are building shared libraries ourselves, then don't include Boost in them
if (BUILD_SHARED_LIBS)
   set(Boost_USE_STATIC_LIBS    OFF)
else ()
   set(Boost_USE_STATIC_LIBS     ON)
endif ()
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

include(cmake/Modules/UseMultiArch.cmake)

# Requires BOOST filesystem version 3, thus 1.44 is necessary.
add_definitions(-DBOOST_FILESYSTEM_VERSION=3)
find_package(Boost 1.44.0 COMPONENTS filesystem date_time system unit_test_framework REQUIRED)
include_directories(${PROJECT_SOURCE_DIR} ${Boost_INCLUDE_DIRS})

# if we are using dynamic boost, the header file must generate a main() function
if (BUILD_SHARED_LIBS)
   add_definitions(-DBOOST_TEST_DYN_LINK)
endif ()


set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
find_package(cjson)
if (HAVE_CJSON)
   include_directories( ${CJSON_INCLUDE_DIR} )   
else()
    include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/opm/json" )   
endif()


add_subdirectory(opm/json)
add_subdirectory(opm/parser)


# These commands should be run unconditionally; i.e. the testdata
# directory should be copied to the EXECUTABLE_OUTPUT_PATH for each
# invocation of cmake, and the the directory generated-source/ should
# be (re)created under the build directory for each invocation. These
# dependencies are currently not correctly handled.

file(COPY ${PROJECT_SOURCE_DIR}/testdata DESTINATION ${EXECUTABLE_OUTPUT_PATH})
file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/generated-source")

