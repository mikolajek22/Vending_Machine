#pragma once

#include <functional>
#include <string>

namespace vending {

class ICardReader {
    public:
        virtual ~ICardReader();
        virtual void setOnCardTapped(std::function<void(const std::string& )> callback) = 0;
};

} /*namspace vending*/