import QtQuick
import QtQuick.Layouts
import PolriApp

Rectangle {
    id: root
    Layout.fillWidth: true
    height: 90
    color: Style.surface
    radius: Style.cardRadius

    property string title: ""
    property string value: ""
    property color valueColor: Style.textPrimary
    property string iconSymbol: "i"

    RowLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15

        // Icon Container
        Rectangle {
            width: 45
            height: 45
            color: "transparent"
            border.color: Style.orangeAccent
            border.width: 1.5
            radius: 22.5
            Text {
                anchors.centerIn: parent
                text: root.iconSymbol
                color: Style.orangeAccent
                font.pixelSize: 18
                font.bold: true
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2
            Text {
                text: root.title.toUpperCase()
                color: Style.textSecondary
                font.pixelSize: 11
                font.letterSpacing: 1
                font.bold: true
            }
            MarqueeText {
                Layout.fillWidth: true
                text: root.value
                color: root.valueColor
                font.pixelSize: 20
                font.bold: true
            }
        }
    }
}
