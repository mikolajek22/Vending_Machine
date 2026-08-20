#pragma once

#include <string>

namespace vending {

class uuid {
    public:
        static std::string generate_v4();
};

} /* namespace vending */