#pragma once

#include <core/iTransport.hpp>
#include <vector>
#include <atomic>
#include <random>
#include <vector>
#include <iostream>

namespace vending {

class FakeTransport : public ITransport
{
    public:
        bool send(const Transaction &t) override {
            float res = _rngDist(_rng);
            std::cout << "Proba. result is: " << res << std::endl;
            bool succeedeed = _forcedResult && res > _failureProbability;
            _online = succeedeed;
            if (succeedeed)
                _sentUuids.push_back(t.uuid);

            return succeedeed;
        }

        /**
         * @brief Force result of send() - only for test
         */
        void forceResult(bool result) { _forcedResult = result; }

        
        bool isOnline() const { return _online; }
        void setFailureProbability(double probability) { _failureProbability = probability; }

        std::vector<std::string> sentUuids() { return _sentUuids; }

    private:
        std::atomic<bool> _online{true};
        double _failureProbability = 0.0;
        std::mt19937 _rng{std::random_device{}()};
        std::uniform_real_distribution<double> _rngDist{0.0, 1.0};

        bool _forcedResult = true;
        /* Successfully sent Transactions (uuid) */
        std::vector<std::string> _sentUuids;
};
}/*namespace vending */