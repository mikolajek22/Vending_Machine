#pragma once

#include <sqlite3.h>
#include <core/iTransactionJournal.hpp>

namespace vending
{
class SqlTransactionJournal : public ITransactionJournal
{
    public:
        explicit SqlTransactionJournal(const std::string &dbPath);
        ~SqlTransactionJournal() override;
        void save(const Transaction& transaction) override;
        void updateStatus(const std::string uuid, const TransactionStatus status) override;
        void syncDone(const std::string &uuid) override;
        std::vector<Transaction> findNotSynced() override;
        std::optional<Transaction> findByUuid(const std::string& uuid) override;
    private:
        void _init();
        void _recovery();
        Transaction _readTow(sqlite3_stmt *stmt);
        sqlite3 *_db = nullptr;
};
}/* namespace vending */