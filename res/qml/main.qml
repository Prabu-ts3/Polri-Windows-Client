import QtQuick
import QtQuick.Window
import QtQuick.Layouts
import QtQuick.Controls
import "components"
import "screens"

Window {
    width: 1200
    height: 800
    visible: true
    title: qsTr("Polri Windows Client - Native")
    color: "#121212"

    // StackLayout Utama untuk mengatur alur Login -> Main App
    StackLayout {
        id: rootStack
        anchors.fill: parent
        currentIndex: 0 // Dimulai dari Halaman Login

        // 1. Halaman Login (Replika activity_login.xml)
        LoginScreen {
            id: loginScreen
            onLoginSuccess: rootStack.currentIndex = 1
        }

        // 2. Tampilan Aplikasi Utama (Setelah Login)
        RowLayout {
            spacing: 0

            // Sidebar (Navigasi Pengganti MenuActivity)
            Sidebar {
                id: mainSidebar
                Layout.fillHeight: true
            }

            // Vertical Divider
            Rectangle {
                width: 1
                Layout.fillHeight: true
                color: "#333333"
            }

            // Area Konten Berdasarkan Pilihan Sidebar (StackLayout Modular)
            StackLayout {
                id: stackLayout
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentIndex: 0

                Dashboard { id: dashboardScreen }
                GroupScreen { id: groupScreen }
                UserScreen { id: userScreen }
                VideoScreen { id: videoScreen }
                MapsScreen { id: mapsScreen }
                SettingsScreen { id: settingsScreen }
                AboutScreen { id: aboutScreen }
            }
        }
    }

    // Fungsi Navigasi Sidebar untuk berpindah halaman di dalam stackLayout
    function switchPage(menuName) {
        if (menuName === "Dashboard") stackLayout.currentIndex = 0
        else if (menuName === "Group") stackLayout.currentIndex = 1
        else if (menuName === "User") stackLayout.currentIndex = 2
        else if (menuName === "Video") stackLayout.currentIndex = 3
        else if (menuName === "Maps") stackLayout.currentIndex = 4
        else if (menuName === "Settings") stackLayout.currentIndex = 5
        else if (menuName === "About") stackLayout.currentIndex = 6
    }

    // Hubungkan sinyal dari Sidebar ke fungsi switchPage
    Component.onCompleted: {
        mainSidebar.menuChanged.connect(switchPage)
    }
}
