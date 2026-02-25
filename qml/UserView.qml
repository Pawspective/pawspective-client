import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    anchors.fill: parent

    QtObject {
        id: theme
        readonly property string fontName: "Comic Sans MS"
        readonly property color pageBg: "#e8d8cb"
        readonly property color sidebarBg: "#e9bebb"
        readonly property color purple: "#b8abd7"
        readonly property color fieldBg: "#fdfdfd"
        readonly property color accentPink: "#f4a7b9"
        readonly property color textDark: "#8572af"
        readonly property color buttonText: "#e7ebf5"
    }

    property string userEmail: "email@example.com"
    property string userFirstName: "Alice"
    property string userLastName: "Brown"

    readonly property real leftPanelWidth: root.width * 0.7
    readonly property real rightPanelWidth: root.width * 0.3

    signal logoutClicked()
    signal editProfileClicked() 

    color: theme.pageBg

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: root.leftPanelWidth
            color: theme.pageBg

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 40
                spacing: 25

                Text {
                    text: "Profile"
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    font.family: theme.fontName
                    font.pixelSize: 32
                    font.bold: true
                    color: theme.textDark
                }

                ProfileDataField { label: "Email"; value: userEmail }
                ProfileDataField { label: "First Name"; value: userFirstName }
                ProfileDataField { label: "Last Name"; value: userLastName }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 20

                    ColumnLayout {
                        spacing: 15
                        Layout.alignment: Qt.AlignLeft

                        CustomButton {
                            text: "Edit Profile"
                            baseColor: theme.purple
                            hoverColor: theme.accentPink
                            textColor: theme.buttonText
                            Layout.preferredWidth: 250
                            Layout.preferredHeight: 60
                            onClicked: root.editProfileClicked()
                        }

                        CustomButton {
                            text: "Logout"
                            baseColor: theme.purple
                            hoverColor: theme.accentPink
                            textColor: theme.buttonText
                            Layout.preferredWidth: 250
                            Layout.preferredHeight: 60
                            onClicked: root.logoutClicked() 
                        }
                    }

                    Item { Layout.fillWidth: true }

                    Image {
                        Layout.preferredWidth: 350
                        Layout.preferredHeight: 350
                        Layout.alignment: Qt.AlignRight
                        source: "../resources/blue_cat.png"
                        fillMode: Image.PreserveAspectFit
                        smooth: true
                    }
                }
                
                Item { Layout.fillHeight: true }
            }
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: root.rightPanelWidth
            color: theme.sidebarBg

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

                Item { Layout.fillHeight: true }
            }
        }
    }

    component ProfileDataField : ColumnLayout {
        property string label: ""
        property string value: ""
        Layout.fillWidth: true
        spacing: 8

        Text {
            text: parent.label
            font.family: theme.fontName
            font.pixelSize: 18
            color: theme.textDark
        }

        Rectangle {
            Layout.fillWidth: true
            height: 45
            radius: 10
            color: theme.fieldBg

            Text {
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.verticalCenter: parent.verticalCenter
                text: parent.parent.value
                font.family: theme.fontName
                font.pixelSize: 22
                color: theme.accentPink
            }
        }
    }

    // Компонент бокового меню
    component SidebarItem : Rectangle {
        property string text: ""
        property bool active: false
        Layout.fillWidth: true
        height: 65
        radius: 8
        color: active ? theme.purple : "transparent"

        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 30
            text: parent.text
            font.family: theme.fontName
            font.pixelSize: 22
            font.bold: active
            color: active ? "white" : theme.textDark
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: if (!parent.active) parent.color = theme.accentPink
            onExited: if (!parent.active) parent.color = "transparent"
        }
    }
}