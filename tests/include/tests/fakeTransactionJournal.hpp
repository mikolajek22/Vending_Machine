#pragma once

#include "core/iTransactionJournal.hpp"
#include <unordered_map>
namespace vending {


class FakeTransactionJournal : public ITransactionJournal {
    public:
        void save(const Transaction& transaction) override
        {
            _transactions[transaction.uuid] = transaction;
        }

        void updateStatus(const std::string uuid, const TransactionStatus status) override 
        {
            auto it = _transactions.find(uuid);
            if (it != _transactions.end())
                it->second.status = status;
        }

        void syncDone(const std::string &uuid) override
        {
            auto it = _transactions.find(uuid);
            if (it != _transactions.end())
                it->second.synced = true;
        }

        std::vector<Transaction> findNotSynced() override
        {
            std::vector<Transaction> ret;
            for (const auto &[uuid, t] : _transactions)
            {
                if (!t.synced)
                    ret.push_back(t);
            }
            return ret;
        }

        std::optional<Transaction> findByUuid(const std::string& uuid) override
        {
            auto it = _transactions.find(uuid);

            if (it != _transactions.end())
                return it->second;
            
            return std::nullopt;
        }

        

    private:
        std::unordered_map<std::string, Transaction> _transactions;
};


} /* namespace vinding*/