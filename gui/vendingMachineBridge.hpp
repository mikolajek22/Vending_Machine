#pragma once

#include <QObject>
#include <QTimer>

#include "tests/fakeCardReader.hpp"
#include "tests/fakeDispenser.hpp"
#include "tests/fakeTransport.hpp"
#include "tests/fakeTransactionJournal.hpp"
#include "core/transportWorker.hpp"
#include "core/vendingManager.hpp"

class VendingMachineBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString state READ state NOTIFY stateChanged)
    Q_PROPERTY(bool online READ online NOTIFY onlineChanged)
    Q_PROPERTY(int pendingSyncCount READ pendingSyncCount NOTIFY pendingSyncCountChanged)

public:
    explicit VendingMachineBridge(QObject* parent = nullptr);
    ~VendingMachineBridge() override;

    QString state() const;
    bool online() const;
    int pendingSyncCount();

    Q_INVOKABLE void simulateCardTap();
    Q_INVOKABLE void selectProduct(const QString& productId);

signals:
    void stateChanged();
    void onlineChanged();
    void pendingSyncCountChanged();

private slots:
    /* periodically called with set interval */
    void tick();

private:
    vending::FakeCardReader         _cardReader;
    vending::FakeDispenser          _dispenser;
    vending::FakeTransactionJournal _journal;
    vending::FakeTransport          _transport;
    vending::VendingManager         _manager;
    vending::TransportWorker        _transportWorker;

    QTimer _tickTimer;

    vending::State _lastState = vending::State::STATE_IDLE;
    bool _lastOnline = true;
    int _lastPendingSyncCount = 0;
};