#include "core/stateMachine.hpp"

namespace vending {

State StateMachine::onCardTapped(const std::string &cardId)
{
    /* Event handled only in this states */
    if (_state == State::STATE_IDLE || _state == State::STATE_COMPLETED || _state == State::STATE_FAILED)
    {
        _cardId = cardId;
        _productId.clear();
        _state = State::STATE_CARD_READ;

        /* Start timer in product selection timeout case */
        _timeoutPoint = std::chrono::steady_clock::now() + _timeout;

    }
    return _state;
}

State StateMachine::onProductSelected(const std::string &productId)
{
    /* Event handled only in this states */
    if (_state == State::STATE_CARD_READ)
    {
        _productId = productId;
        _state = State::STATE_PRODUCT_SELECTED;
    }
    return _state;
}

State StateMachine::onBeginDispensing()
{
    if (_state == State::STATE_PRODUCT_SELECTED)
        _state = State::STATE_DISPENSING;
    return _state;
}

State StateMachine::onDispenseResult(DispenseResult result)
{
    if (_state == State::STATE_DISPENSING)
    {
        _state = (result == DispenseResult::RESULT_OK) ? State::STATE_COMPLETED : State::STATE_FAILED;
    }
    return _state;
}

State StateMachine::checkTimeout()
{
    if (_state == State::STATE_CARD_READ && std::chrono::steady_clock::now() >= _timeoutPoint)
    {
        _state = State::STATE_IDLE;
        _cardId.clear();
    }
    return _state;
}
} /* namespace vending */