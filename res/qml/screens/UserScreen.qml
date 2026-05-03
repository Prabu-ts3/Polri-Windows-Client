import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import PolriApp
import "../components"

Item {
    id: userScreen
    anchors.fill: parent

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Style.spacing
        spacing: 15

        Text {
            text: "ONLINE USERS"
            color: Style.orangeAccent
            font.pixelSize: 24
            font.bold: true
        }

        // Search Bar Placeholder
        TextField {
            Layout.fillWidth: true
            placeholderText: "Search user..."
            color: "white"
            background: Rectangle {
                color: Style.surface
                radius: 8
            }
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: wsManager.usersOnline
            spacing: 10
            clip: true

            delegate: Rectangle {
                width: parent.width
                height: 70
                color: Style.surface
                radius: Style.cardRadius

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 15

                    // Avatar Placeholder
                    Rectangle {
                        width: 40; height: 40; radius: 20
                        color: modelData.is_speaking ? Style.orangeAccent : "#444444"
                        Text {
                            anchors.centerIn: parent
                            text: modelData.name ? modelData.name.charAt(0).toUpperCase() : "?"
                            color: "white"
                            font.bold: true
                        }

                        // Green Dot for Online
                        Rectangle {
                            anchors.bottom: parent.bottom
                            anchors.right: parent.right
                            width: 12; height: 12; radius: 6
                            color: Style.success
                            border.color: Style.surface
                            border.width: 2
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        Text {
                            text: modelData.name || "Unknown User"
                            color: "white"
                            font.bold: true
                            font.pixelSize: 16
                        }
                        Text {
                            text: "ID: " + (modelData.id || "N/A")
                            color: Style.textColor
                            font.pixelSize: 12
                            opacity: 0.6
                        }
                    }

                    // PTP Button
                    Button {
                        text: "Private"
                        visible: modelData.id !== wsManager.myUserId // Assuming we expose myUserId
                        onClicked: {
                            wsManager.startPtp(modelData.id, modelData.name)
                            // Navigate to dashboard to see PTP status
                            root.currentScreen = "Dashboard.qml"
                        }
                    }

                    // Speaking Indicator
                    if (modelData.is_speaking) {
                        Text {
                            text: "🎙️"
                            font.pixelSize: 18
                        }
                    }
                }
            }
        }
    }
}
