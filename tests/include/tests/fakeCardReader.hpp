#pragma once

#include <core/iCardReader.hpp>

namespace vending {

class FakeCardReader : public ICardReader
{
    public:
        void setOnCardTapped(std::function<void(const std::string& )> callback) override
        {
            _cb = callback;
        }

        void simulateCardTap(const std::string & cardId)
        {
            if (_cb)
            {
                _cb(cardId);
            }
        }
    private:
        std::function<void(const std::string& )> _cb;
};

} /*namespace vending*/