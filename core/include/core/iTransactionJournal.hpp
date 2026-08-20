#pragma once

#include "core/transaction.hpp"

#include <optional>
#include <vector>
namespace vending {


class ITransactionJournal {
    public:
        virtual ~ITransactionJournal() = default;
        virtual void save(const Transaction& transaction) = 0;
        virtual void updateStatus(const std::string uuid, const TransactionStatus status) = 0;
        virtual void syncDone(const std::string &uuid) = 0;

        virtual std::vector<Transaction> findNotSynced() = 0;
        virtual std::optional<Transaction> findByUuid(const std::string& uuid) = 0;

    private:
};


} /* namespace vinding*/