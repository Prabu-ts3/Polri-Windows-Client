pragma Singleton
import QtQuick

QtObject {
    // Colors
    readonly property color background: "#121212"
    readonly property color surface: "#1E1E1E"
    readonly property color orangeAccent: "#FF9800"
    readonly property color textPrimary: "#FFFFFF"
    readonly property color textSecondary: "#BBBBBB"
    readonly property color divider: "#333333"
    readonly property color danger: "#FF5252"
    readonly property color success: "#00FF00"

    // Sizes
    readonly property int sidebarWidth: 260
    readonly property int cardRadius: 12
    readonly property int spacing: 16
}
