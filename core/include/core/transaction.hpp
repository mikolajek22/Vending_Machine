#pragma once

#include <string>
#include <chrono>

namespace vending {

enum class TransactionStatus
{
    STATUS_OK,
    STATUS_FAIL,
    STATUS_PENDING,
    STATUS_UNKNOWN,
};

struct Transaction {
    std::string uuid;
    std::string prductId;
    std::string cardId;
    TransactionStatus status;
    std::chrono::system_clock::time_point timestamp;

    /* is transaction synchronized with backend???*/
    bool synced;
};

} /* namespace vending */