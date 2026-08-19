#pragma once

#include <core/iCardReader.hpp>
#include <core/iDispenser.hpp>
#include <core/stateMachine.hpp>

namespace vending {

class VendingManager {
    public:
        VendingManager(ICardReader &cardReader, IDispenser  &dispenser, std::chrono::seconds timeout = std::chrono::seconds(15));
        void selectProduct(const std::string & productId);
        /* Periodically called */
        void checkTimeout();
        State currentState();
    private:
        StateMachine    _sm;
        IDispenser      &_dispenser;
};

} /* namespoace vendding */