#pragma once

#include <core/iTransport.hpp>
#include <vector>


namespace vending {

class FakeTransport : public ITransport
{
    public:
        bool send(const Transaction &t) override {
            if (_forcedResult)
                _sentUuids.push_back(t.uuid);

            return _forcedResult;
        }

        /**
         * @brief Force result of send() - only for test
         */
        void forceResult(bool result) { _forcedResult = result; }
        std::vector<std::string> sentUuids() { return _sentUuids; }
    private:
        bool _forcedResult = true;
        /* Successfully sent Transactions (uuid) */
        std::vector<std::string> _sentUuids;
};
}/*namespace vending */