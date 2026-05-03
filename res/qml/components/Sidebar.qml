import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: sidebar
    width: 260
    height: parent.height
    color: "#1E1E1E"

    property string currentSelection: "Dashboard"
    signal menuChanged(string menuName)

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // App Logo Section (Watermark-like)
        Rectangle {
            Layout.fillWidth: true
            height: 120
            color: "transparent"
            Column {
                anchors.centerIn: parent
                spacing: 5
                Text {
                    text: "POLRI"
                    color: "#FF9800"
                    font.pixelSize: 28
                    font.bold: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Text {
                    text: "PTT CLIENT"
                    color: "white"
                    font.pixelSize: 10
                    opacity: 0.6
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }

        // Menu Items dari activity_menu.xml
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ColumnLayout {
                width: parent.width
                spacing: 0

                SidebarItem {
                    label: "Dashboard"
                    isSelected: sidebar.currentSelection === "Dashboard"
                    onClicked: { sidebar.currentSelection = "Dashboard"; sidebar.menuChanged("Dashboard") }
                }

                SidebarItem {
                    label: "Group"
                    isSelected: sidebar.currentSelection === "Group"
                    onClicked: { sidebar.currentSelection = "Group"; sidebar.menuChanged("Group") }
                }

                SidebarItem {
                    label: "User"
                    isSelected: sidebar.currentSelection === "User"
                    onClicked: { sidebar.currentSelection = "User"; sidebar.menuChanged("User") }
                }

                SidebarItem {
                    label: "Video"
                    isSelected: sidebar.currentSelection === "Video"
                    onClicked: { sidebar.currentSelection = "Video"; sidebar.menuChanged("Video") }
                }

                SidebarItem {
                    label: "Maps"
                    isSelected: sidebar.currentSelection === "Maps"
                    onClicked: { sidebar.currentSelection = "Maps"; sidebar.menuChanged("Maps") }
                }

                SidebarItem {
                    label: "Settings"
                    isSelected: sidebar.currentSelection === "Settings"
                    onClicked: { sidebar.currentSelection = "Settings"; sidebar.menuChanged("Settings") }
                }

                SidebarItem {
                    label: "About"
                    isSelected: sidebar.currentSelection === "About"
                    onClicked: { sidebar.currentSelection = "About"; sidebar.menuChanged("About") }
                }
            }
        }

        // Bottom Section
        Rectangle {
            Layout.fillWidth: true
            height: 40
            color: "transparent"
            Row {
                anchors.centerIn: parent
                spacing: 8
                Rectangle {
                    width: 10
                    height: 10
                    radius: 5
                    color: wsManager.connectionStatus === 2 ? "#4CAF50" : (wsManager.connectionStatus === 1 ? "#FFC107" : "#F44336")
                }
                Text {
                    text: wsManager.connectionStatus === 2 ? "ONLINE" : (wsManager.connectionStatus === 1 ? "CONNECTING..." : "OFFLINE")
                    color: "white"
                    font.pixelSize: 10
                    font.bold: true
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#333333"
        }

        SidebarItem {
            label: "Logout"
            textColor: "#FF5252"
            onClicked: console.log("Logout clicked")
        }
    }
}
