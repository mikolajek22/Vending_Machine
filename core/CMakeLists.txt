find_package(SQLite3 REQUIRED)

add_library(core
    src/stateMachine.cpp
    src/vendingManager.cpp
    src/uuid.cpp
    src/sqlTransactionJournal.cpp
)

target_include_directories(core PUBLIC include)
target_link_libraries(core PUBLIC SQLite::SQLite3)