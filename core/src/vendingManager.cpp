#include <core/vendingManager.hpp>

namespace vending {

VendingManager::VendingManager(ICardReader &cardReader, IDispenser  &dispenser, std::chrono::seconds timeout) 
                                : _dispenser(dispenser), _sm(timeout)
{
    cardReader.setOnCardTapped([this](const std::string& cardId) {
        _sm.onCardTapped(cardId);
    });
}

void VendingManager::selectProduct(const std::string & productId)
{
    if (State::STATE_PRODUCT_SELECTED != _sm.onProductSelected(productId))
        return; // Ignore - wrong sequence

    _sm.onBeginDispensing();

    _dispenser.dispense(productId, [this](DispenseResult result) {
        _sm.onDispenseResult(result);
    });
}
void VendingManager::checkTimeout()
{
    _sm.checkTimeout();
}
State VendingManager::currentState()
{
    return _sm.currentState();
}

} /*namespac e vending*/