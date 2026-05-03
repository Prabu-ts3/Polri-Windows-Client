import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtMultimedia
import PolriApp
import "../components"

Item {
    id: videoScreen
    anchors.fill: parent

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Style.spacing
        spacing: Style.spacing

        Text {
            text: "LIVE VIDEO STREAM"
            color: Style.orangeAccent
            font.pixelSize: 24
            font.bold: true
        }

        GridLayout {
            columns: 2
            Layout.fillWidth: true
            Layout.fillHeight: true
            columnSpacing: Style.spacing
            rowSpacing: Style.spacing

            // Main Video Feed (Connected to C++ VideoRenderer)
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "black"
                radius: Style.cardRadius
                clip: true

                VideoOutput {
                    id: mainVideoOutput
                    anchors.fill: parent
                    fillMode: VideoOutput.PreserveAspectFit

                    // Hubungkan sink dari VideoOutput ini ke C++ VideoRenderer
                    Component.onCompleted: {
                        videoRenderer.videoSink = mainVideoOutput.videoSink
                    }
                }

                Rectangle {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.margins: 10
                    width: 70
                    height: 25
                    color: Style.danger
                    radius: 4
                    RowLayout {
                        anchors.centerIn: parent
                        spacing: 4
                        Rectangle { width: 8; height: 8; radius: 4; color: "white" }
                        Text {
                            text: "LIVE"
                            color: "white"
                            font.pixelSize: 10
                            font.bold: true
                        }
                    }
                }

                Text {
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.margins: 10
                    text: "Main Feed - Incoming WebSocket Stream"
                    color: "white"
                    font.pixelSize: 12
                    opacity: 0.7
                }
            }

            // Placeholders for other feeds
            Repeater {
                model: 3
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: Style.surface
                    radius: Style.cardRadius

                    Text {
                        anchors.centerIn: parent
                        text: "CAMERA " + (index + 2) + "\n(OFFLINE)"
                        color: "#444444"
                        horizontalAlignment: Text.AlignHCenter
                        font.bold: true
                    }
                }
            }
        }
    }
}
