#include <core/vendingManager.hpp>
#include <core/uuid.hpp>

namespace vending {

VendingManager::VendingManager(ICardReader &cardReader, IDispenser  &dispenser, ITransactionJournal &transactionJournal, std::chrono::seconds timeout) 
                                : _dispenser(dispenser), _transactionJournal(transactionJournal), _sm(timeout)
{
    cardReader.setOnCardTapped([this](const std::string& cardId) {
        _sm.onCardTapped(cardId);
    });
}

void VendingManager::selectProduct(const std::string & productId)
{
    if (State::STATE_PRODUCT_SELECTED != _sm.onProductSelected(productId))
        return; // Ignore - wrong sequence

    /* Save tranasction before dispensing */
    Transaction t = {
        .uuid = uuid::generate_v4(),
        .prductId = productId,
        .cardId = _sm.currentCardId(),
        .status = TransactionStatus::STATUS_PENDING,
        .timestamp = std::chrono::system_clock::now(),
        .synced = false,
    };
    _currentTransactionUuid = t.uuid;
    _transactionJournal.save(t);
    
    _sm.onBeginDispensing();

    _dispenser.dispense(productId, [this](DispenseResult result) {
        /* Actualize transaction status in DB */
        TransactionStatus status = (result == DispenseResult::RESULT_OK) ? TransactionStatus::STATUS_OK : TransactionStatus::STATUS_FAIL;
        _transactionJournal.updateStatus(_currentTransactionUuid, status);
        
        _sm.onDispenseResult(result);
    });
}
void VendingManager::checkTimeout()
{
    _sm.checkTimeout();
}
State VendingManager::currentState() const
{
    return _sm.currentState();
}

} /*namespac e vending*/