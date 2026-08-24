#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "vendingMachineBridge.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    VendingMachineBridge bridge;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("bridge", &bridge);
    engine.load(QUrl(QStringLiteral("qrc:/VendingMachine/gui/qml/main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}