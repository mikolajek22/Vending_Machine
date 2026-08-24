cmake_minimum_required(VERSION 3.16)
project(Vending_Machine CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
 
add_subdirectory(core)

find_package(Qt6 REQUIRED COMPONENTS Quick)
qt_standard_project_setup(REQUIRES 6.4)

qt_add_executable(Vending_Machine
    gui/main.cpp
    gui/vendingMachineBridge.cpp
)

qt_add_qml_module(Vending_Machine
    URI VendingMachine
    VERSION 1.0
    QML_FILES gui/qml/main.qml gui/qml/MainScreen.qml
)

target_include_directories(Vending_Machine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/include
)

target_link_libraries(Vending_Machine PRIVATE core Qt6::Quick)

enable_testing()
find_package(GTest REQUIRED)

add_executable(tests 
    tests/test_stateMachine.cpp
    tests/test_vendingManager.cpp
    tests/test_transportWorker.cpp
)

target_include_directories(tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/include
)

target_link_libraries(tests PRIVATE 
    core 
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tests)