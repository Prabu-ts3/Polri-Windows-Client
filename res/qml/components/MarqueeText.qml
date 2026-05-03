import QtQuick
import PolriApp

Item {
    id: root
    height: innerText.height
    clip: true

    property alias text: innerText.text
    property alias color: innerText.color
    property alias font: innerText.font
    property int interval: 30

    Text {
        id: innerText
        x: 0
        anchors.verticalCenter: parent.verticalCenter

        NumberAnimation on x {
            id: anim
            from: 0
            to: -innerText.width
            duration: innerText.width * root.interval
            loops: Animation.Infinite
            running: innerText.width > root.width
        }

        onWidthChanged: {
            if (width > root.width) anim.restart()
            else x = 0
        }
    }

    // Duplicate text for seamless loop
    Text {
        text: innerText.text
        color: innerText.color
        font: innerText.font
        visible: innerText.width > root.width
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: innerText.right
        anchors.leftMargin: 50
    }
}
