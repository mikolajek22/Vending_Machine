import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: mainScreen

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        Label {
            Layout.alignment: Qt.AlignHCenter
            text: "Stan: " + bridge.state
            font.bold: true
        }

        Button {
            Layout.fillWidth: true
            text: "Symuluj przyłożenie karty"
            onClicked: bridge.simulateCardTap()
        }

        GridLayout {
            columns: 2
            columnSpacing: 8
            rowSpacing: 8
            Layout.fillWidth: true

            Button {
                Layout.fillWidth: true
                text: "cola"
                enabled: bridge.state === "CardRead"
                onClicked: bridge.selectProduct("cola")
            }
            Button {
                Layout.fillWidth: true
                text: "water"
                enabled: bridge.state === "CardRead"
                onClicked: bridge.selectProduct("water")
            }
            Button {
                Layout.fillWidth: true
                text: "beer"
                enabled: bridge.state === "CardRead"
                onClicked: bridge.selectProduct("beer")
            }
            Button {
                Layout.fillWidth: true
                text: "juice"
                enabled: bridge.state === "CardRead"
                onClicked: bridge.selectProduct("juice")
            }
        }

        BusyIndicator {
            Layout.alignment: Qt.AlignHCenter
            running: bridge.state === "Dispensing"
            visible: running
        }

        Item { Layout.fillHeight: true }

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: bridge.online ? "online" : "offline"
            }

            Item { Layout.fillWidth: true }

            Label {
                text: "Do synchronizacji: " + bridge.pendingSyncCount
            }
        }
    }
}