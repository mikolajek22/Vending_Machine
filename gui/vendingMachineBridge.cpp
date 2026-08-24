#include "vendingMachineBridge.hpp"

namespace {

QString stateToString(vending::State state) {
    switch (state) {
        case vending::State::STATE_IDLE:                return "Idle";
        case vending::State::STATE_CARD_READ:           return "CardRead";
        case vending::State::STATE_PRODUCT_SELECTED:    return "ProductSelected";
        case vending::State::STATE_DISPENSING:          return "Dispensing";
        case vending::State::STATE_COMPLETED:           return "Completed";
        case vending::State::STATE_FAILED:              return "Failed";
    }
    return "Unknown";
}

constexpr int const_DispenseDelayMs              = 2000;      // symulacja czasu fizycznego wydania
/* polling interval to display state of macuine */
constexpr int const_TickIntervalMs               = 250;        // co ile pollujemy stan/timeout
/* REST backend server is not available for most of the time - realized by propability of succeeded action of send() through transporter */
constexpr double const_BackendFailureProbability = 0.7;
/* sync probe - interval  */
constexpr int const_transportWorkerInterval = 1000;
/* default card id */
constexpr std::string const_catdId = "CARD_1";

}  /* namespace */

VendingMachineBridge::VendingMachineBridge(QObject* parent)
    : QObject(parent), _journal("vending.db") ,_manager(_cardReader, _dispenser, _journal) , _transportWorker(_journal, _transport)
{
    _transport.setFailureProbability(const_BackendFailureProbability);
    _transportWorker.start(std::chrono::milliseconds(const_transportWorkerInterval));

    connect(&_tickTimer, &QTimer::timeout, this, &VendingMachineBridge::tick);
    _tickTimer.start(const_TickIntervalMs);
}

VendingMachineBridge::~VendingMachineBridge() {
    _transportWorker.stop();
}

QString VendingMachineBridge::state() const {
    return stateToString(_manager.currentState());
}

bool VendingMachineBridge::online() const {
    return _transport.isOnline();
}

int VendingMachineBridge::pendingSyncCount() {
    return static_cast<int>(_journal.findNotSynced().size());
}

void VendingMachineBridge::simulateCardTap() {
    _cardReader.simulateCardTap(const_catdId);
    emit stateChanged();
}

void VendingMachineBridge::selectProduct(const QString& productId) {
    _manager.selectProduct(productId.toStdString());
    emit stateChanged();

    if (_manager.currentState() == vending::State::STATE_DISPENSING) {
        // NON BLOCKING, finish() will be called after const_DispenseDelayMs!
        QTimer::singleShot(const_DispenseDelayMs, this, [this]() {
            _dispenser.finish(vending::DispenseResult::RESULT_OK);
        });
    }
}

void VendingMachineBridge::tick() {
    _manager.checkTimeout();

    /* Check state machine */
    vending::State current = _manager.currentState();
    if (current != _lastState) {
        _lastState = current;
        emit stateChanged();
    }

    /* Check if dev is online */
    bool currentOnline = _transport.isOnline();
    if (currentOnline != _lastOnline) {
        _lastOnline = currentOnline;
        emit onlineChanged();
    }

    /* display pending transactions */
    int currentPending = pendingSyncCount();
    if (currentPending != _lastPendingSyncCount) {
        _lastPendingSyncCount = currentPending;
        emit pendingSyncCountChanged();
    }
}