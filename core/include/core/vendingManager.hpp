#pragma once

#include <core/iCardReader.hpp>
#include <core/iDispenser.hpp>

#include <core/iTransactionJournal.hpp>
#include <core/stateMachine.hpp>

namespace vending {

class VendingManager {
    public:
        VendingManager(ICardReader &cardReader, IDispenser  &dispenser, 
                       ITransactionJournal &transactionJournal, std::chrono::seconds timeout = std::chrono::seconds(15));
        void selectProduct(const std::string & productId);
        /* Periodically called */
        void checkTimeout();
        State currentState();
    private:
        StateMachine            _sm;
        IDispenser              &_dispenser;
        ITransactionJournal     &_transactionJournal;

        std::string _currentTransactionUuid;

};

} /* namespoace vendding */