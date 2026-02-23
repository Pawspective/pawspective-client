import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

Rectangle {
    id: root
    anchors.fill: parent

    readonly property color pageColor: "#e8d8cb"
    readonly property color sidebarColor: "#e9bebb"
    readonly property color purpleColor: "#b8abd7"
    readonly property color fieldColor: "#e7ebf5"
    readonly property color buttonColor: "#b8abd7"
    readonly property color hoverColor: "#f4a7b9"

    color: pageColor

    property string userEmail: "email@example.com"
    property string userFirstName: "Alice"
    property string userLastName: "Brown"

    readonly property real leftPanelWidth: root.width * 0.7
    readonly property real rightPanelWidth: root.width * 0.3

    RowLayout {
        anchors.fill: parent
        spacing: 0

         Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: root.leftPanelWidth
            color: pageColor

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 40
                spacing: 25

                Text {
                    text: "Profile"
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.family: "Comic Sans MS"
                    font.pixelSize: 32
                    font.bold: true
                    color: "#8572af"
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "Email"
                        font.family: "Comic Sans MS"
                        font.pixelSize: 18
                        color: "#8572af"
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 45
                        radius: 10
                        color: fieldColor

                        Text {
                            anchors.left: parent.left
                            anchors.leftMargin: 20
                            anchors.verticalCenter: parent.verticalCenter
                            text: userEmail
                            font.family: "Comic Sans MS"
                            font.pixelSize: 22
                            color: "#f4a7b9"
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "First Name"
                        font.family: "Comic Sans MS"
                        font.pixelSize: 18
                        color: "#8572af"
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 45
                        radius: 10
                        color: fieldColor

                        Text {
                            anchors.left: parent.left
                            anchors.leftMargin: 20
                            anchors.verticalCenter: parent.verticalCenter
                            text: userFirstName
                            font.family: "Comic Sans MS"
                            font.pixelSize: 22
                            color: "#f4a7b9"
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "Last Name"
                        font.family: "Comic Sans MS"
                        font.pixelSize: 18
                        color: "#8572af"
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 45
                        radius: 10
                        color: fieldColor

                        Text {
                            anchors.left: parent.left
                            anchors.leftMargin: 20
                            anchors.verticalCenter: parent.verticalCenter
                            text: userLastName
                            font.family: "Comic Sans MS"
                            font.pixelSize: 22
                            color: "#f4a7b9"
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 20

                    ColumnLayout {
                        spacing: 15
                        Layout.alignment: Qt.AlignLeft

                        Rectangle {
                            id: editButton
                            Layout.preferredWidth: 250
                            Layout.preferredHeight: 60
                            radius: 14
                            color: buttonColor

                            Text {
                                anchors.centerIn: parent
                                text: "Edit Profile"
                                font.family: "Comic Sans MS"
                                font.pixelSize: 18
                                color: "#e7ebf5"
                            }

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true

                                onEntered: editButton.color = hoverColor
                                onExited: editButton.color = buttonColor
                                onClicked: console.log("Edit clicked")
                            }
                        }

                        Rectangle {
                            id: logoutButton
                            Layout.preferredWidth: 250
                            Layout.preferredHeight: 60
                            radius: 14
                            color: buttonColor

                            Text {
                                anchors.centerIn: parent
                                text: "Logout"
                                font.family: "Comic Sans MS"
                                font.pixelSize: 18
                                color: "#e7ebf5"
                            }

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true

                                onEntered: logoutButton.color = hoverColor
                                onExited: logoutButton.color = buttonColor
                                onClicked: console.log("Logout clicked")
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    Image {
                        Layout.preferredWidth: 350
                        Layout.preferredHeight: 350
                        Layout.alignment: Qt.AlignRight
                        source: "../resources/blue_cat.png"
                        fillMode: Image.PreserveAspectFit
                        smooth: true
                    }
                }
                
                Item {
                    Layout.fillHeight: true
                }
            }
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: root.rightPanelWidth
            color: sidebarColor

            ColumnLayout {
                anchors.fill: parent
                anchors.topMargin: 80
                spacing: 20

                SidebarItem { text: "Profile"; active: true }
                SidebarItem { text: "Search" }
                SidebarItem { text: "Organization" }

                Image {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.preferredWidth: Math.min(parent.width * 0.8, 400)
                    Layout.preferredHeight: 900
                    Layout.topMargin: -180
                    source: "../resources/lilac.png"
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                }

                Item { 
                    Layout.fillHeight: true 
                }
            }
        }
    }

    component SidebarItem : Rectangle {
        property string text: ""
        property bool active: false

        Layout.fillWidth: true
        height: 65
        radius: 8

        color: active ? root.purpleColor : "transparent"

        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 30
            text: parent.text
            font.family: "Comic Sans MS"
            font.pixelSize: 22
            font.bold: active
            color: active ? "white" : "#8572af"
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true

            onEntered: {
                if (!parent.active) parent.color = root.hoverColor
            }

            onExited: {
                if (!parent.active) parent.color = "transparent"
            }
        }
    }
}