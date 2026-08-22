#include <gtest/gtest.h>

#include <tests/fakeTransport.hpp>
#include <tests/fakeTransactionJournal.hpp>
#include <core/transportWorker.hpp>

#include "core/uuid.hpp"    

using namespace vending;


TEST(TransportWorker, SyncAllUnsynced_Success)
{
    FakeTransactionJournal ftj;
    FakeTransport ft;
    TransportWorker tw(ftj, ft);
    Transaction t = {
        .uuid = "uuid_1",
        .prductId = "cola",
        .cardId = "card_1",
        .status = TransactionStatus::STATUS_OK,
        .timestamp = std::chrono::system_clock::now(),
        .synced = false,
    };

    ftj.save(t);
    t.uuid = "uuid_2";
    ftj.save(t);
    bool syncStatus = tw.sync();

    EXPECT_TRUE(syncStatus);
    EXPECT_EQ(ft.sentUuids().size(), 2U);
    EXPECT_EQ(ftj.findNotSynced().empty(), true);
}

TEST(TransportWorker, SyncAllUnsynced_Fail)
{
    FakeTransactionJournal ftj;
    FakeTransport ft;
    TransportWorker tw(ftj, ft);
    Transaction t = {
        .uuid = "uuid_1",
        .prductId = "cola",
        .cardId = "card_1",
        .status = TransactionStatus::STATUS_OK,
        .timestamp = std::chrono::system_clock::now(),
        .synced = false,
    };

    ftj.save(t);
    t.uuid = "uuid_2";
    ftj.save(t);
    ft.forceResult(false);
    bool syncStatus = tw.sync();

    EXPECT_FALSE(syncStatus);
    EXPECT_EQ(ft.sentUuids().size(), 0U);
    EXPECT_EQ(ftj.findNotSynced().size(), 2U);
}

TEST(TransportWorker, SyncAllUnsynced_FailToSuccess)
{
    FakeTransactionJournal ftj;
    FakeTransport ft;
    TransportWorker tw(ftj, ft);
    Transaction t = {
        .uuid = "uuid_1",
        .prductId = "cola",
        .cardId = "card_1",
        .status = TransactionStatus::STATUS_OK,
        .timestamp = std::chrono::system_clock::now(),
        .synced = false,
    };

    ftj.save(t);
    t.uuid = "uuid_2";
    ftj.save(t);

    /* Simulate server connection lost */
    ft.forceResult(false);
    bool syncStatus = tw.sync();

    EXPECT_FALSE(syncStatus);
    EXPECT_EQ(ft.sentUuids().size(), 0U);
    EXPECT_EQ(ftj.findNotSynced().size(), 2U);

    /* Connectiom came bacl */
    ft.forceResult(true);
    syncStatus = tw.sync();

    EXPECT_TRUE(syncStatus);
    EXPECT_EQ(ft.sentUuids().size(), 2U);
    EXPECT_EQ(ftj.findNotSynced().size(), 0U);
}