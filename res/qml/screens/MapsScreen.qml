import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtLocation
import QtPositioning
import PolriApp
import "../components"

Item {
    id: mapsScreen
    anchors.fill: parent

    Plugin {
        id: mapPlugin
        name: "osm" // OpenStreetMap
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Style.spacing
        spacing: Style.spacing

        Text {
            text: "MAPS / TRACKING"
            color: Style.orangeAccent
            font.pixelSize: 24
            font.bold: true
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: Style.surface
            radius: Style.cardRadius
            clip: true

            Map {
                id: map
                anchors.fill: parent
                plugin: mapPlugin
                center: QtPositioning.coordinate(-6.2088, 106.8456) // Jakarta default
                zoomLevel: 14

                MapItemView {
                    model: locationManager.userLocations
                    delegate: MapQuickItem {
                        coordinate: QtPositioning.coordinate(modelData.latitude, modelData.longitude)
                        anchorPoint.x: markerImage.width / 2
                        anchorPoint.y: markerImage.height

                        sourceItem: Column {
                            spacing: 2
                            Text {
                                text: modelData.userName
                                color: "white"
                                font.bold: true
                                style: Text.Outline
                                styleColor: "black"
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                            Rectangle {
                                id: markerImage
                                width: 24
                                height: 24
                                radius: 12
                                color: Style.orangeAccent
                                border.color: "white"
                                border.width: 2
                            }
                        }
                    }
                }
            }

            // Map Overlay Controls
            Column {
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: 20
                spacing: 10

                Button {
                    text: "+"
                    width: 40
                    height: 40
                    onClicked: map.zoomLevel++
                }
                Button {
                    text: "-"
                    width: 40
                    height: 40
                    onClicked: map.zoomLevel--
                }
            }
        }
    }
}
