import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import PolriApp
import "../components"

Item {
    id: groupScreen
    anchors.fill: parent

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Style.spacing
        spacing: 15

        Text {
            text: "CHANNELS & GROUPS"
            color: Style.orangeAccent
            font.pixelSize: 24
            font.bold: true
        }

        ListView {
            id: channelListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: wsManager.channelsList
            spacing: 12
            clip: true

            delegate: Rectangle {
                width: channelListView.width
                height: 80
                color: modelData.slug === wsManager.channelName ? "#1A3A5A" : Style.surface
                radius: Style.cardRadius
                border.color: modelData.slug === wsManager.channelName ? Style.primaryColor : "transparent"
                border.width: 2

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        wsManager.joinChannel(modelData.slug)
                    }
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 15

                    // Group Icon
                    Rectangle {
                        width: 50; height: 50; radius: 25
                        color: modelData.slug === wsManager.channelName ? Style.primaryColor : "#333333"
                        Text {
                            anchors.centerIn: parent
                            text: "📢"
                            font.pixelSize: 24
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        Text {
                            text: modelData.name || "Unnamed Group"
                            color: "white"
                            font.bold: true
                            font.pixelSize: 18
                        }
                        Text {
                            text: (modelData.member_count || 0) + " Members Online"
                            color: Style.textColor
                            font.pixelSize: 12
                            opacity: 0.6
                        }
                    }

                    // Active Indicator
                    Rectangle {
                        visible: modelData.slug === wsManager.channelName
                        width: 80
                        height: 30
                        radius: 15
                        color: Style.success
                        Text {
                            anchors.centerIn: parent
                            text: "ACTIVE"
                            color: "white"
                            font.bold: true
                            font.pixelSize: 10
                        }
                    }
                }
            }
        }
    }
}
