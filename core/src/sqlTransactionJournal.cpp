#include <core/sqlTransactionJournal.hpp>

#include <stdexcept>

namespace vending {

static int64_t convert_to_unixEpoch(std::chrono::system_clock::time_point timestamp)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(timestamp.time_since_epoch()).count();
}

static std::chrono::system_clock::time_point convert_to_timestamp(int64_t ms)
{
    return std::chrono::system_clock::time_point(std::chrono::milliseconds(ms));
}


void SqlTransactionJournal::_init()
{
    const std::string initQuerry = 
        "CREATE TABLE IF NOT EXISTS transactions ("
        " uuid TEXT PRIMARY KEY,"
        " cardId TEXT NOT NULL,"
        " productId TEXT NOT NULL,"
        " timestamp INTEGER NOT NULL,"
        " status INTEGER NOT NULL,"
        " synced INTEGER NOT NULL DEFAULT 0"
        ");";
    
    char* err = nullptr;
    if (SQLITE_OK != sqlite3_exec(_db, initQuerry.c_str(), nullptr, nullptr, &err))
    {
        std::string msg = err ? err : "unknown";
        sqlite3_free(err);
        throw std::runtime_error("Cannot create table, error code: " + msg);
    }
}

void SqlTransactionJournal::_recovery()
{
    const char *querry = "UPDATE transactions SET status = ?, synced = 0 WHERE status = ?";
    sqlite3_stmt *stmt = nullptr;
    // -1 copies querry till found '/0'
    sqlite3_prepare_v2(_db, querry, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, static_cast<int>(TransactionStatus::STATUS_UNKNOWN));
    sqlite3_bind_int(stmt, 2, static_cast<int>(TransactionStatus::STATUS_PENDING));
    // proceed
    sqlite3_step(stmt);
    // free resources
    sqlite3_finalize(stmt);
}

Transaction SqlTransactionJournal::_readTow(sqlite3_stmt *stmt)
{
    Transaction t;
    t.uuid = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    t.cardId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    t.prductId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    t.timestamp = convert_to_timestamp(sqlite3_column_int64(stmt, 3));
    t.status = static_cast<TransactionStatus>(sqlite3_column_int(stmt, 4));
    t.synced = static_cast<bool>(sqlite3_column_int(stmt, 5));
    return t;
}

SqlTransactionJournal::SqlTransactionJournal(const std::string &dbPath)
{
    if (SQLITE_OK != sqlite3_open(dbPath.c_str(), &_db))
    {
        std::string msg = (_db != nullptr) ? sqlite3_errmsg(_db) : "unknown";
        throw std::runtime_error("Faile do open sqlite3 db, error code: " + msg);
    }

    /*todo create table */
    _init();
    _recovery();

}

SqlTransactionJournal::~SqlTransactionJournal()
{
    if (_db)
        sqlite3_close(_db);
}

void SqlTransactionJournal::save(const Transaction& transaction)
{
    const char *querry = 
            "INSERT INTO transactions (uuid, cardId, productId, timestamp, status, synced)"
            "VALUES (?, ?, ?, ?, ?, ?);";
    
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(_db, querry, -1, &stmt, nullptr);
    // TRANSIENT to not lose string date after leaving method
    sqlite3_bind_text(stmt, 1, transaction.uuid.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, transaction.cardId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, transaction.prductId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 4, convert_to_unixEpoch(transaction.timestamp));
    sqlite3_bind_int(stmt, 5, static_cast<int>(transaction.status));
    sqlite3_bind_int(stmt, 6, static_cast<int>(0)); //force set as unsynced while saving transaction to db

    // proceed
    if (SQLITE_DONE != sqlite3_step(stmt))
    {
        std::string err = sqlite3_errmsg(_db);
        sqlite3_finalize(stmt);
        throw std::runtime_error("Error while saving transaction, code: " + err);
    }
    // free resources
    sqlite3_finalize(stmt);

}
void SqlTransactionJournal::updateStatus(const std::string uuid, const TransactionStatus status)
{
    const char *querry = "UPDATE transactions SET status = ?, synced = 0 WHERE uuid = ?;";
    sqlite3_stmt *stmt = nullptr;

    sqlite3_prepare_v2(_db, querry, -1, &stmt, nullptr);

    sqlite3_bind_int(stmt, 1, static_cast<int>(status));
    sqlite3_bind_text(stmt, 2, uuid.c_str(), -1, SQLITE_TRANSIENT);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

}
void SqlTransactionJournal::syncDone(const std::string &uuid)
{
    const char *querry = "UPDATE transactions SET synced = 1 WHERE uuid = ?;";
    sqlite3_stmt *stmt = nullptr;

    sqlite3_prepare_v2(_db, querry, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, uuid.c_str(), -1, SQLITE_TRANSIENT);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

std::vector<Transaction> SqlTransactionJournal::findNotSynced()
{
    const char *querry =    "SELECT uuid, cardId, productId, timestamp, status, synced "
                            "FROM transactions WHERE synced = 0;";
    sqlite3_stmt *stmt = nullptr;
    sqlite3_prepare_v2(_db, querry, -1, &stmt, nullptr);

    std::vector<Transaction> ret;

    while (sqlite3_step(stmt) == SQLITE_ROW)
        ret.push_back(_readTow(stmt));

    sqlite3_finalize(stmt);
    return ret;
}
std::optional<Transaction> SqlTransactionJournal::findByUuid(const std::string& uuid)
{
    const char *querry =    "SELECT uuid, cardId, productId, timestamp, status, synced "
                            "FROM transactions WHERE uuid = ?;";

    sqlite3_stmt *stmt = nullptr;
    sqlite3_prepare_v2(_db, querry, -1, &stmt, nullptr);

    sqlite3_bind_text(stmt, 1, uuid.c_str(), -1, SQLITE_TRANSIENT);

    std::optional<Transaction> ret;
    if (sqlite3_step(stmt) == SQLITE_ROW)
        ret = _readTow(stmt);
    
    sqlite3_finalize(stmt);
    return ret;
}
} /*namespace vending */