import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import PolriApp
import "../components"

Item {
    id: dashboardRoot
    anchors.fill: parent

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Style.spacing
        spacing: 20

        // Header Info
        RowLayout {
            Layout.fillWidth: true
            ColumnLayout {
                Text {
                    text: "CHANNEL"
                    color: Style.textColor
                    font.pixelSize: 12
                    opacity: 0.6
                }
                RowLayout {
                    Text {
                        text: wsManager.channelName
                        color: Style.primaryColor
                        font.pixelSize: 24
                        font.bold: true
                    }
                    Button {
                        text: "End PTP"
                        visible: wsManager.isPtpActive
                        onClicked: wsManager.endPtp()
                        background: Rectangle {
                            color: Style.danger
                            radius: 4
                        }
                    }
                }
            }
            Item { Layout.fillWidth: true }
            InfoCard {
                icon: "👤"
                title: "STATUS"
                value: wsManager.connected ? "Online" : "Offline"
                valueColor: wsManager.connected ? Style.success : Style.danger
            }
        }

        // Active Speaker Card
        Rectangle {
            Layout.fillWidth: true
            height: 80
            color: Style.surface
            radius: Style.cardRadius
            border.color: wsManager.talkingStatus !== "IDLE" ? Style.orangeAccent : "transparent"
            border.width: 2

            RowLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 15

                Rectangle {
                    width: 40; height: 40; radius: 20
                    color: wsManager.talkingStatus !== "IDLE" ? Style.orangeAccent : "#333333"
                    Text {
                        anchors.centerIn: parent
                        text: "🎙️"
                        font.pixelSize: 20
                    }
                }

                ColumnLayout {
                    Text {
                        text: "CURRENT SPEAKER"
                        color: Style.textColor
                        font.pixelSize: 10
                        opacity: 0.6
                    }
                    Text {
                        text: wsManager.talkingStatus
                        color: "white"
                        font.pixelSize: 18
                        font.bold: true
                    }
                }
            }
        }

        // Center Area (PTT Button)
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            PttButton {
                anchors.centerIn: parent
                isPressed: false // Will be handled by internal mouse area or keys

                // Integrasi dengan C++
                onPressedChanged: {
                    if (pressed) {
                        wsManager.startTalking()
                    } else {
                        wsManager.stopTalking()
                    }
                }
            }
        }

        // Bottom Stats
        RowLayout {
            Layout.fillWidth: true
            spacing: Style.spacing

            InfoCard {
                Layout.fillWidth: true
                icon: "📶"
                title: "Latency"
                value: "24 ms"
            }
            InfoCard {
                Layout.fillWidth: true
                icon: "👥"
                title: "Users"
                value: "12 Online"
            }
        }
    }
}
