import QtQuick
import QtQuick.Controls

Item {
    id: root
    width: parent.width
    height: 50

    property string label: ""
    property string iconSource: ""
    property bool isSelected: false
    property color textColor: "white"
    signal clicked()

    Rectangle {
        anchors.fill: parent
        color: isSelected ? "#33FFFFFF" : "transparent"

        Row {
            anchors.fill: parent
            anchors.leftMargin: 20
            spacing: 15
            verticalAlignment: Text.AlignVCenter

            // Simbol sederhana untuk icon sementara
            Text {
                text: "•"
                color: isSelected ? "#FF9800" : root.textColor
                font.pixelSize: 24
                anchors.verticalCenter: parent.verticalCenter
            }

            Text {
                text: root.label
                color: isSelected ? "#FF9800" : root.textColor
                font.pixelSize: 16
                font.bold: isSelected
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: if(!isSelected) parent.color = "#11FFFFFF"
            onExited: if(!isSelected) parent.color = "transparent"
            onClicked: root.clicked()
        }
    }
}
