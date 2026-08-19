#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include <tests/fakeDispenser.hpp>
#include <tests/fakeCardReader.hpp>

#include <core/vendingManager.hpp>

using namespace vending;

/* base tests */
TEST(VendingManager, VM_simulateCardTap)
{
    FakeCardReader fcr;
    FakeDispenser fd;
    VendingManager vm(fcr, fd);

    fcr.simulateCardTap("card 1");
    EXPECT_EQ(vm.currentState(), State::STATE_CARD_READ);
}

TEST(VendingManager, VM_simulateProductDispensing)
{
    FakeCardReader fcr;
    FakeDispenser fd;
    VendingManager vm(fcr, fd);

    fcr.simulateCardTap("card 1");
    vm.selectProduct("cola");
    EXPECT_EQ(vm.currentState(), State::STATE_DISPENSING);
    EXPECT_EQ(fd.get_productId(), "cola");
}

TEST(VendingManager, VM_simulateProductDispensingCompleted)
{
    FakeCardReader fcr;
    FakeDispenser fd;
    VendingManager vm(fcr, fd);

    fcr.simulateCardTap("card 1");
    vm.selectProduct("cola");
    fd.finish(DispenseResult::RESULT_OK);
    EXPECT_EQ(vm.currentState(), State::STATE_COMPLETED);
}

TEST(VendingManager, VM_simulateProductDispensingFailed)
{
    FakeCardReader fcr;
    FakeDispenser fd;
    VendingManager vm(fcr, fd);

    fcr.simulateCardTap("card 1");
    vm.selectProduct("cola");
    fd.finish(DispenseResult::RESULT_FAIL);
    EXPECT_EQ(vm.currentState(), State::STATE_FAILED);
}

/* duoble card tap */
TEST(VendingManager, VM_simulateDoubleCardTap)
{
    FakeCardReader fcr;
    FakeDispenser fd;
    VendingManager vm(fcr, fd);

    fcr.simulateCardTap("card 1");
    vm.selectProduct("cola");
    fcr.simulateCardTap("card 2");
    EXPECT_EQ(vm.currentState(), State::STATE_DISPENSING);
    EXPECT_EQ(fd.get_productId(), "cola");
}

/* no card */
TEST(VendingManager, VM_simulateSelectNoCard)
{
    FakeCardReader fcr;
    FakeDispenser fd;
    VendingManager vm(fcr, fd);

    vm.selectProduct("cola");
    EXPECT_EQ(vm.currentState(), State::STATE_IDLE);
    EXPECT_EQ(fd.get_productId(), "");
}

/* timeout */
TEST(VendingManager, VM_simulateTimeout)
{
    FakeCardReader fcr;
    FakeDispenser fd;
    VendingManager vm(fcr, fd, std::chrono::seconds(1));

    fcr.simulateCardTap("card 1");
    vm.checkTimeout();
    EXPECT_EQ(vm.currentState(), State::STATE_CARD_READ);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    vm.checkTimeout();
    EXPECT_EQ(vm.currentState(), State::STATE_IDLE);
}