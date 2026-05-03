import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import PolriApp

Item {
    id: aboutScreen
    anchors.fill: parent

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 30
        width: parent.width * 0.6

        Rectangle {
            Layout.alignment: Qt.AlignCenter
            width: 150; height: 150
            color: "transparent"
            border.color: Style.orangeAccent
            border.width: 2
            radius: 75

            Text {
                anchors.centerIn: parent
                text: "POLRI"
                color: Style.orangeAccent
                font.pixelSize: 32
                font.bold: true
            }
        }

        ColumnLayout {
            Layout.alignment: Qt.AlignCenter
            spacing: 5
            Text {
                text: "POLRI PTT NATIVE CLIENT"
                color: "white"
                font.pixelSize: 24
                font.bold: true
                Layout.alignment: Qt.AlignCenter
            }
            Text {
                text: "Version 1.0.0 (Windows Native)"
                color: Style.textSecondary
                font.pixelSize: 14
                Layout.alignment: Qt.AlignCenter
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: Style.divider
        }

        Text {
            text: "Aplikasi ini digunakan untuk koordinasi internal kepolisian menggunakan protokol Push-To-Talk via UDP Streaming."
            color: Style.textSecondary
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            font.pixelSize: 14
        }

        Text {
            text: "© 2026 POLRI - Republik Indonesia"
            color: Style.orangeAccent
            font.pixelSize: 12
            Layout.alignment: Qt.AlignCenter
        }
    }
}
