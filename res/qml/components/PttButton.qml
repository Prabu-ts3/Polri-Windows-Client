import QtQuick
import QtQuick.Controls

Item {
    id: root
    property real size: 200
    width: size
    height: size

    signal pressed()
    signal released()

    Rectangle {
        id: buttonCircle
        anchors.fill: parent
        radius: width / 2
        color: pttMouseArea.pressed ? "#FF9800" : "#333333"
        border.color: pttMouseArea.pressed ? "#FFA726" : "#444444"
        border.width: 4

        Behavior on color { ColorAnimation { duration: 100 } }

        Text {
            anchors.centerIn: parent
            text: "PTT"
            color: pttMouseArea.pressed ? "black" : "white"
            font.pixelSize: root.size * 0.2
            font.bold: true
        }

        MouseArea {
            id: pttMouseArea
            anchors.fill: parent
            onPressed: root.pressed()
            onReleased: root.released()
        }
    }

    // Glow effect when pressed (optional but nice for native feel)
    Rectangle {
        anchors.fill: parent
        radius: width / 2
        color: "transparent"
        border.color: "#FF9800"
        border.width: 10
        visible: pttMouseArea.pressed
        opacity: 0.3
        scale: 1.1
    }
}
