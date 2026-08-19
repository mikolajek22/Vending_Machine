#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include <core/stateMachine.hpp>

using namespace vending;

/* Basic */

TEST(StateMachine, IdleStart)
{
    StateMachine sm;
    EXPECT_EQ(sm.currentState(), State::STATE_IDLE);
}

TEST(StateMachine, CompleteTransaction)
{
    StateMachine sm;
    EXPECT_EQ(sm.onCardTapped("card 1"), State::STATE_CARD_READ);
    EXPECT_EQ(sm.onProductSelected("snack"), State::STATE_PRODUCT_SELECTED);
    EXPECT_EQ(sm.onBeginDispensing(), State::STATE_DISPENSING);
    EXPECT_EQ(sm.onDispenseResult(DispenseResult::RESULT_OK), State::STATE_COMPLETED);
}

/* Allows second card after fail */

TEST(StateMachine, SecondCardAfterFail)
{
    StateMachine sm;
    EXPECT_EQ(sm.onCardTapped("card 1"), State::STATE_CARD_READ);
    EXPECT_EQ(sm.onProductSelected("snack"), State::STATE_PRODUCT_SELECTED);
    EXPECT_EQ(sm.onBeginDispensing(), State::STATE_DISPENSING);
    EXPECT_EQ(sm.onDispenseResult(DispenseResult::RESULT_FAIL), State::STATE_FAILED);
    EXPECT_EQ(sm.onCardTapped("card 2"), State::STATE_CARD_READ);
}

/* Select product with no card */

TEST(StateMachine, NoCardDetected)
{
    StateMachine sm;
    EXPECT_EQ(sm.onProductSelected("snack"), State::STATE_IDLE);
}

/* Double Card use cases*/

TEST(StateMachine, IgnoreSecondCardWhileCardAlreadyRead)
{
    StateMachine sm;
    EXPECT_EQ(sm.onCardTapped("card 1"), State::STATE_CARD_READ);
    EXPECT_EQ(sm.onCardTapped("card 2"), State::STATE_CARD_READ);
    EXPECT_EQ(sm.currentCardId(), "card 1");
}

TEST(StateMachine, IgnoreSecondCardWhileProductIsSelected)
{
    StateMachine sm;
    EXPECT_EQ(sm.onCardTapped("card 1"), State::STATE_CARD_READ);
    EXPECT_EQ(sm.onProductSelected("snack"), State::STATE_PRODUCT_SELECTED);
    EXPECT_EQ(sm.onCardTapped("card 2"), State::STATE_PRODUCT_SELECTED);
    EXPECT_EQ(sm.currentCardId(), "card 1");
}


TEST(StateMachine, IgnoreSecondCardWhileSelecting)
{
    StateMachine sm;
    EXPECT_EQ(sm.onCardTapped("card 1"), State::STATE_CARD_READ);
    EXPECT_EQ(sm.onProductSelected("snack"), State::STATE_PRODUCT_SELECTED);
    EXPECT_EQ(sm.onBeginDispensing(), State::STATE_DISPENSING);
    EXPECT_EQ(sm.onCardTapped("card 2"), State::STATE_DISPENSING);
}

TEST(StateMachine, IgnoreSecondCardWhileDispensing)
{
    StateMachine sm;
    EXPECT_EQ(sm.onCardTapped("card 1"), State::STATE_CARD_READ);
    EXPECT_EQ(sm.onProductSelected("snack"), State::STATE_PRODUCT_SELECTED);
    EXPECT_EQ(sm.onBeginDispensing(), State::STATE_DISPENSING);
    EXPECT_EQ(sm.onCardTapped("card 2"), State::STATE_DISPENSING);
}

/* Timeout*/

TEST(StateMachine, SelectTimeout)
{
    StateMachine sm(std::chrono::seconds(1));
    EXPECT_EQ(sm.onCardTapped("card 1"), State::STATE_CARD_READ);

    std::this_thread::sleep_for(std::chrono::seconds(2));
    EXPECT_EQ(sm.checkTimeout(), State::STATE_IDLE);
}

TEST(StateMachine, SelectWithDelay)
{
    StateMachine sm(std::chrono::seconds(3));
    EXPECT_EQ(sm.onCardTapped("card 1"), State::STATE_CARD_READ);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    EXPECT_EQ(sm.checkTimeout(), State::STATE_CARD_READ);
    EXPECT_EQ(sm.onProductSelected("cola"), State::STATE_PRODUCT_SELECTED);
    EXPECT_EQ(sm.checkTimeout(), State::STATE_PRODUCT_SELECTED);
}

