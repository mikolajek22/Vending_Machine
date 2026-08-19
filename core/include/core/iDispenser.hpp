#pragma once

#include <string>
#include <functional>

#include "core/stateMachine.hpp"

namespace vending {

class IDispenser {
    public:
        virtual ~IDispenser();
        virtual void dispense(const std::string& productId, std::function<void(DispenseResult)> onComplete) = 0;
};

} /*namespace vending*/