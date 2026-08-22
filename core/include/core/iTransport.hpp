#pragma once

#include <core/transaction.hpp>

namespace vending {

class ITransport {
    public:
        ~ITransport() = default;

        virtual bool send(const Transaction &t) = 0;
};

}/* namespace venfing*/