cmake_minimum_required (VERSION 2.6)

project(pepper)

include_directories(${MY_ROOT}/include)

file(GLOB_RECURSE CC_SOURCE *.cpp)

add_library(${PROJECT_NAME} ${CC_SOURCE})

set(LIBRARY_OUTPUT_PATH ${MY_LIB_DIR})
