import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import PolriApp

Rectangle {
    id: loginRoot
    anchors.fill: parent
    color: "black"

    signal loginSuccess()

    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(parent.width * 0.8, 400)
        spacing: 20

        // Logo
        Rectangle {
            Layout.alignment: Qt.AlignCenter
            width: 100; height: 100
            color: "transparent"
            border.color: Style.orangeAccent
            border.width: 2
            radius: 50
            Text {
                anchors.centerIn: parent
                text: "POLRI"
                color: Style.orangeAccent
                font.pixelSize: 20
                font.bold: true
            }
        }

        Text {
            Layout.alignment: Qt.AlignCenter
            text: "RESTA-SLEMAN"
            color: "white"
            font.pixelSize: 22
            font.bold: true
        }

        // Server Status & URL Display
        RowLayout {
            Layout.alignment: Qt.AlignCenter
            spacing: 8
            Rectangle {
                width: 10; height: 10; radius: 5
                color: wsManager.isConnected ? Style.success : Style.danger
            }
            Text {
                text: wsManager.isConnected ? "Server Connected" : "Server Offline"
                color: wsManager.isConnected ? Style.success : Style.danger
                font.bold: true
                font.pixelSize: 12
            }
        }

        // Server URL Input (Added for flexibility)
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 5
            Text { text: "SERVER URL"; color: "#888888"; font.pixelSize: 10; font.bold: true }
            TextField {
                id: serverUrlField
                Layout.fillWidth: true
                height: 40
                text: settingsManager.serverUrl
                placeholderText: "ws://103.210.68.106:3000"
                color: "white"
                background: Rectangle {
                    color: "#222222"
                    radius: 4
                    border.color: "#444444"
                }
                onTextChanged: settingsManager.serverUrl = text
            }
        }

        // Username Input
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 5
            Text { text: "USERNAME"; color: "#888888"; font.pixelSize: 10; font.bold: true }
            TextField {
                id: usernameField
                Layout.fillWidth: true
                height: 45
                placeholderText: "Masukkan Username"
                color: "black"
                background: Rectangle { color: "white"; radius: 4 }
            }
        }

        // Password Input
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 5
            Text { text: "PASSWORD"; color: "#888888"; font.pixelSize: 10; font.bold: true }
            TextField {
                id: passwordField
                Layout.fillWidth: true
                height: 45
                placeholderText: "Masukkan Password"
                echoMode: TextInput.Password
                color: "black"
                background: Rectangle { color: "white"; radius: 4 }
            }
        }

        Button {
            id: loginBtn
            Layout.fillWidth: true
            Layout.height: 50
            Layout.marginTop: 10
            contentItem: Text {
                text: "LOGIN"
                color: "white"
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            background: Rectangle {
                color: loginBtn.pressed ? "#E68A00" : Style.orangeAccent
                radius: 4
            }
            onClicked: {
                wsManager.login(usernameField.text, passwordField.text, serverUrlField.text)
            }
        }

        Connections {
            target: wsManager
            function onLoginSuccess() {
                loginRoot.loginSuccess()
            }
            function onLoginError(message) {
                errorText.text = message
                errorText.visible = true
            }
        }

        Text {
            id: errorText
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            color: Style.danger
            visible: false
            font.pixelSize: 12
        }
    }
}
