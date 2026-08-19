add_library(core
    src/stateMachine.cpp
    src/vendingManager.cpp
)

target_include_directories(core PUBLIC include)