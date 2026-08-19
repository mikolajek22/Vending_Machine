#pragma once

#include <chrono>
#include <string>

#define DEFAULT_TIMEOUT     15 // in sec

namespace vending {

enum class State {
    STATE_IDLE,
    STATE_CARD_READ,
    STATE_PRODUCT_SELECTED,
    STATE_DISPENSING,
    STATE_COMPLETED,
    STATE_FAILED,
};

enum class DispenseResult
{
    RESULT_OK,
    RESULT_FAIL,
};

class StateMachine {
    public:
        explicit StateMachine(std::chrono::seconds timeout = std::chrono::seconds(DEFAULT_TIMEOUT)) : _timeout(timeout) {};

        State onCardTapped(const std::string &cardId);
        State onProductSelected(const std::string &productId);
        State onDispenseResult(DispenseResult result);

        State onBeginDispensing();
        State checkTimeout();
        State currentState() const { return _state; }
        std::string currentCardId() const { return _cardId; }
        std::string currentProductId() const { return _productId; }
        

    private:
        State _state = State::STATE_IDLE;
        std::string _cardId;
        std::string _productId;

        std::chrono::seconds _timeout;  // value of timeout in second after card detection
        std::chrono::steady_clock::time_point _timeoutPoint; // after crossing this point - timeout
};

} /* namespace vending */