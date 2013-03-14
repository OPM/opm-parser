cmake_minimum_required (VERSION 2.6)
project (OPMParser CXX)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
SET(CMAKE_BUILD_TYPE "debug")
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG -pg -Wall -fopenmp -ggdb")

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost 1.45.0 COMPONENTS filesystem system REQUIRED)

include_directories(${PROJECT_SOURCE_DIR} ${Boost_INCLUDE_DIRS})

enable_testing()
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
add_subdirectory(eclipse)
