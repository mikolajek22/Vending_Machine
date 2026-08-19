#pragma once

#include "core/iDispenser.hpp"

namespace vending {


class FakeDispenser : public IDispenser
{
    public:
        void dispense(const std::string& productId, std::function<void(DispenseResult)> onComplete) override
        {
            _productId = productId;
            _cb = std::move(onComplete);
        }

        void finish(DispenseResult result)
        {
            if (_cb)
            {
                auto cb = _cb;
                /* dispensing done, clear callback */
                _cb = nullptr;
                cb(result);
            }
        }

        std::string get_productId() const {return _productId; }
        bool isDispensing() const { return (_cb != nullptr) ? true : false; }
    private:
        std::string _productId;
        std::function<void(DispenseResult)> _cb;
};


}

