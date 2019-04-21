# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.8)
project(unit-test)
SET(CMAKE_BUILD_TYPE Debug)

################################
# GTest
################################

if(MSVC)
  # By default, GTest compiles on Windows in CRT static linkage mode. We use this
  # variable to force it into using the CRT in dynamic linkage (DLL), just as CPR
  # does.
  set(gtest_force_shared_crt ON CACHE BOOL "Force gtest to use the shared c runtime")
endif()

ADD_SUBDIRECTORY(googletest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)

################################
# Unit Tests
################################

# Add test cpp file
add_executable(runUnitTests ${CMAKE_CURRENT_SOURCE_DIR}/unit-tests/csv_test.cpp  
                            ${CMAKE_CURRENT_SOURCE_DIR}/unit-tests/main.cpp)
                            
# Link test executable against gtest & gtest_main
target_link_libraries(runUnitTests gtest gtest_main)
add_test(runUnitTests runUnitTests)