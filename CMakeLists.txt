cmake_minimum_required(VERSION 3.16)
project(Vending_Machine CXX)
 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
 
add_subdirectory(core)

add_executable(Vending_Machine main.cpp)

target_link_libraries(Vending_Machine PRIVATE core)

enable_testing()
find_package(GTest REQUIRED)

add_executable(tests tests/test_stateMachine.cpp)
target_link_libraries(tests PRIVATE core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)