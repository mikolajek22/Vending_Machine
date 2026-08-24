#include <gtest/gtest.h>

#include <filesystem>

#include <core/sqlTransactionJournal.hpp>
#include <core/uuid.hpp>

using namespace vending;

namespace {
std::string tempDbPath(const std::string& name) {
    return "/tmp/vending_test_" + name + ".db";
}
}  // namespace

TEST(sqlTransactionJournal, SaveAndFinDyUuid)
{
    std::string dbPath = tempDbPath("journal");
    std::filesystem::remove(dbPath);
    std::string tempUuid = uuid::generate_v4();

    Transaction t {
        .uuid = tempUuid,
        .prductId = "cola",
        .cardId = "card_1",
        .status = TransactionStatus::STATUS_PENDING,
        .timestamp = std::chrono::system_clock::now(),
        .synced = false,
    };

    SqlTransactionJournal journal(dbPath);
    journal.save(t);

    auto read_t = journal.findByUuid(tempUuid);
    ASSERT_TRUE(read_t.has_value());

    EXPECT_EQ(read_t->prductId, "cola");
    EXPECT_EQ(read_t->cardId, "card_1");
    EXPECT_EQ(read_t->status, TransactionStatus::STATUS_PENDING);
    EXPECT_EQ(read_t->synced, false);

}

TEST(sqlTransactionJournal, UpdateStatus)
{
    std::string dbPath = tempDbPath("journal");
    std::filesystem::remove(dbPath);
    std::string tempUuid = uuid::generate_v4();

    Transaction t {
        .uuid = tempUuid,
        .prductId = "cola",
        .cardId = "card_1",
        .status = TransactionStatus::STATUS_PENDING,
        .timestamp = std::chrono::system_clock::now(),
        .synced = false,
    };

    SqlTransactionJournal journal(dbPath);
    journal.save(t);

    journal.updateStatus(tempUuid, TransactionStatus::STATUS_OK);
    

    auto read_t = journal.findByUuid(tempUuid);
    ASSERT_TRUE(read_t.has_value());
    EXPECT_EQ(read_t->status, TransactionStatus::STATUS_OK);

}

TEST(sqlTransactionJournal, FindUnsynced)
{
    std::string dbPath = tempDbPath("journal");
    std::filesystem::remove(dbPath);

    Transaction t1 {
        .uuid = uuid::generate_v4(),
        .prductId = "cola",
        .cardId = "card_1",
        .status = TransactionStatus::STATUS_PENDING,
        .timestamp = std::chrono::system_clock::now(),
        .synced = false,
    };
    
    Transaction t2 {
        .uuid = uuid::generate_v4(),
        .prductId = "cola",
        .cardId = "card_1",
        .status = TransactionStatus::STATUS_PENDING,
        .timestamp = std::chrono::system_clock::now(),
        .synced = false,
    };

    SqlTransactionJournal journal(dbPath);
    journal.save(t1);
    journal.save(t2);

    auto unsynced = journal.findNotSynced();
    EXPECT_EQ(unsynced.size(), 2u);
    journal.syncDone(unsynced[0].uuid);
    unsynced = journal.findNotSynced();
    EXPECT_EQ(unsynced.size(), 1u);
}

TEST(sqlTransactionJournal, Recovery)
{
    std::string tempUuid = uuid::generate_v4();
    std::string dbPath = tempDbPath("journal");
    std::filesystem::remove(dbPath);
    /* Simulate open, save and reset */
    {
        Transaction t {
            .uuid = tempUuid,
            .prductId = "cola",
            .cardId = "card_1",
            .status = TransactionStatus::STATUS_PENDING,
            .timestamp = std::chrono::system_clock::now(),
            .synced = false,
        };

        SqlTransactionJournal journal(dbPath);
        journal.save(t);
        auto read_t = journal.findByUuid(tempUuid);
        EXPECT_EQ(read_t->status, TransactionStatus::STATUS_PENDING);
    }
    
    SqlTransactionJournal journal(dbPath);
    auto read_t = journal.findByUuid(tempUuid);

    EXPECT_EQ(read_t->status, TransactionStatus::STATUS_UNKNOWN);

}