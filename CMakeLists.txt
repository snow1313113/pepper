CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

project(pepper_root)

# color
if(NOT WIN32)
  string(ASCII 27 Esc)
  set(ColourReset "${Esc}[m")
  set(ColourBold  "${Esc}[1m")
  set(Red         "${Esc}[31m")
  set(Green       "${Esc}[32m")
  set(Yellow      "${Esc}[33m")
  set(Blue        "${Esc}[34m")
  set(Magenta     "${Esc}[35m")
  set(Cyan        "${Esc}[36m")
  set(White       "${Esc}[37m")
  set(BoldRed     "${Esc}[1;31m")
  set(BoldGreen   "${Esc}[1;32m")
  set(BoldYellow  "${Esc}[1;33m")
  set(BoldBlue    "${Esc}[1;34m")
  set(BoldMagenta "${Esc}[1;35m")
  set(BoldCyan    "${Esc}[1;36m")
  set(BoldWhite   "${Esc}[1;37m")
endif()

SET(MY_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
SET(MY_LIB_DIR ${MY_ROOT}/lib)

# can use your own googletest directory here
SET(GTEST_ROOT ${MY_ROOT}/third_party/googletest)
SET(GTEST_INCLUDE ${GTEST_ROOT}/googletest/include)
SET(GTEST_LIB ${GTEST_ROOT}/build/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -fPIC -std=c++14")

add_subdirectory(src)
add_subdirectory(sample)
add_subdirectory(test)
