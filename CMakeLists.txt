cmake_minimum_required (VERSION 2.6)
project (OPMParser CXX)

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
SET(CMAKE_BUILD_TYPE "debug")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG -pg -Wall -fopenmp -ggdb")
option( INCLUDE_STATOIL_TESTS  "Include the tests which need proprietary Statoil data" OFF)

if (BOOST_ROOT AND NOT DEFINED Boost_NO_SYSTEM_PATHS)
   set (Boost_NO_SYSTEM_PATHS TRUE)
endif (BOOST_ROOT AND NOT DEFINED Boost_NO_SYSTEM_PATHS)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

include(cmake/Modules/UseMultiArch.cmake)

# Requires BOOST filesystem version 3, thus 1.44 is necessary.
add_definitions(-DBOOST_FILESYSTEM_VERSION=3)
find_package(Boost 1.44.0 COMPONENTS filesystem date_time system unit_test_framework REQUIRED)
include_directories(${PROJECT_SOURCE_DIR} ${Boost_INCLUDE_DIRS})

enable_testing()

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

