import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../"

Rectangle {
    color: Style.backgroundColor

    Flickable {
        anchors.fill: parent
        contentHeight: contentLayout.height + 60
        clip: true

        ColumnLayout {
            id: contentLayout
            width: parent.width - 60
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 30
            spacing: 25

            Text {
                text: "Settings"
                font.pixelSize: 28
                font.bold: true
                color: Style.primaryColor
            }

            // --- Network Section ---
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 10

                Text {
                    text: "Network Configuration"
                    font.pixelSize: 18
                    font.bold: true
                    color: Style.textColor
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 2
                    color: Style.primaryColor
                    opacity: 0.3
                }

                Text {
                    text: "Server WebSocket URL"
                    color: Style.textColor
                    opacity: 0.7
                }

                TextField {
                    id: serverUrlField
                    Layout.fillWidth: true
                    text: settingsManager.serverUrl
                    placeholderText: "ws://ip-address:port"
                    color: "white"
                    background: Rectangle {
                        color: "#2A2A2A"
                        radius: 8
                        border.color: serverUrlField.activeFocus ? Style.primaryColor : "transparent"
                    }
                }
            }

            // --- Audio Section ---
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 15

                Text {
                    text: "Audio & PTT Mode"
                    font.pixelSize: 18
                    font.bold: true
                    color: Style.textColor
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 2
                    color: Style.primaryColor
                    opacity: 0.3
                }

                // Volume
                ColumnLayout {
                    Layout.fillWidth: true
                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: "Output Volume"; color: Style.textColor; Layout.fillWidth: true }
                        Text { text: Math.round(volumeSlider.value) + "%"; color: Style.primaryColor; font.bold: true }
                    }
                    Slider {
                        id: volumeSlider
                        Layout.fillWidth: true
                        from: 0; to: 100
                        value: settingsManager.outputVolume
                        onMoved: settingsManager.outputVolume = value
                    }
                }

                // VOX Configuration
                RowLayout {
                    Layout.fillWidth: true
                    ColumnLayout {
                        Layout.fillWidth: true
                        Text { text: "VOX (Voice Activated)"; color: "white"; font.bold: true }
                        Text { text: "Otomatis bicara saat suara terdeteksi"; color: Style.textColor; font.pixelSize: 11; opacity: 0.6 }
                    }
                    Switch {
                        checked: settingsManager.voxEnabled
                        onToggled: settingsManager.voxEnabled = checked
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    enabled: settingsManager.voxEnabled
                    opacity: enabled ? 1.0 : 0.3
                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: "VOX Threshold"; color: Style.textColor; Layout.fillWidth: true }
                        Text { text: Math.round(voxSlider.value); color: Style.orangeAccent; font.bold: true }
                    }
                    Slider {
                        id: voxSlider
                        Layout.fillWidth: true
                        from: 500; to: 10000
                        value: settingsManager.voxThreshold
                        onMoved: settingsManager.voxThreshold = value
                    }
                }

                // Gateway Mode
                RowLayout {
                    Layout.fillWidth: true
                    ColumnLayout {
                        Layout.fillWidth: true
                        Text { text: "Gateway Mode"; color: "white"; font.bold: true }
                        Text { text: "Selalu aktifkan audio input untuk repeater"; color: Style.textColor; font.pixelSize: 11; opacity: 0.6 }
                    }
                    Switch {
                        checked: settingsManager.gatewayMode
                        onToggled: settingsManager.gatewayMode = checked
                    }
                }

                // --- Hardware Section ---
                Text {
                    text: "Audio Hardware"
                    font.pixelSize: 18
                    font.bold: true
                    color: Style.textColor
                    Layout.topMargin: 10
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 2
                    color: Style.primaryColor
                    opacity: 0.3
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 5
                    Text { text: "Audio Input (Microphone)"; color: Style.textColor; opacity: 0.7 }
                    ComboBox {
                        id: inputCombo
                        Layout.fillWidth: true
                        model: settingsManager.availableInputs
                        currentIndex: model.indexOf(settingsManager.inputDeviceId)
                        onActivated: settingsManager.inputDeviceId = currentText

                        background: Rectangle {
                            color: "#2A2A2A"
                            radius: 8
                        }
                        contentItem: Text {
                            text: inputCombo.displayText
                            color: "white"
                            verticalAlignment: Text.AlignVCenter
                            leftPadding: 10
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 5
                    Text { text: "Audio Output (Speaker)"; color: Style.textColor; opacity: 0.7 }
                    ComboBox {
                        id: outputCombo
                        Layout.fillWidth: true
                        model: settingsManager.availableOutputs
                        currentIndex: model.indexOf(settingsManager.outputDeviceId)
                        onActivated: settingsManager.outputDeviceId = currentText

                        background: Rectangle {
                            color: "#2A2A2A"
                            radius: 8
                        }
                        contentItem: Text {
                            text: outputCombo.displayText
                            color: "white"
                            verticalAlignment: Text.AlignVCenter
                            leftPadding: 10
                        }
                    }
                }
            }

            Button {
                id: saveButton
                text: "Save & Apply Changes"
                Layout.alignment: Qt.AlignRight
                Layout.preferredWidth: 200
                Layout.preferredHeight: 45
                Layout.topMargin: 20

                property bool showSuccess: false

                contentItem: Text {
                    text: saveButton.showSuccess ? "✓ Settings Saved" : saveButton.text
                    color: "white"
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle {
                    color: saveButton.showSuccess ? "#4CAF50" : (parent.down ? "#1A3A5A" : Style.primaryColor)
                    radius: 8
                    Behavior on color { ColorAnimation { duration: 200 } }
                }

                onClicked: {
                    // Update URL (Ini akan memicu signal serverUrlChanged di C++)
                    settingsManager.serverUrl = serverUrlField.text

                    // Trigger feedback visual
                    showSuccess = true
                    successTimer.start()
                    console.log("Settings updated in real-time")
                }

                Timer {
                    id: successTimer
                    interval: 2000
                    onTriggered: saveButton.showSuccess = false
                }
            }
        }
    }
}
