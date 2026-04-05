import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    color: "#e8d8cb"

    signal profileRequested()
    signal organizationClicked(int organizationId)

    QtObject {
        id: theme
        readonly property string fontName: "Comic Sans MS"
        readonly property color sidebarBg: "#e9bebb"
        readonly property color purple: "#b8abd7"
        readonly property color accentPink: "#f4a7b9"
        readonly property color textDark: "#8572af"
        readonly property color buttonText: "#e7ebf5"
        readonly property color fieldBg: "#fdfdfd"
    }

    readonly property real leftPanelWidth: root.width * 0.7
    readonly property real rightPanelWidth: root.width * 0.3
    readonly property real contentMargins: root.height * 0.05

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Main content
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: root.leftPanelWidth
            color: root.color

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: root.contentMargins
                spacing: root.height * 0.025

                Text {
                    text: "Search"
                    font.family: theme.fontName
                    font.pixelSize: root.height * 0.05
                    font.bold: true
                    color: theme.textDark
                    Layout.fillWidth: true
                }

                ScrollView {
                    id: cardsScroll
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    contentWidth: cardsScroll.width

                    Flow {
                        width: cardsScroll.width
                        spacing: 12
                        padding: 4

                        OrganizationCardView {
                            organizationName: "LOL"
                            organizationCity: "Moscow"
                            organizationDescription: "Дырбулщищ"
                            organizationId: 1
                            width: (parent.width - 28) / 2
                            onClicked: function(id) { root.organizationClicked(id) }
                        }

                        OrganizationCardView {
                            organizationName: "KEK"
                            organizationCity: "Kazan"
                            organizationId: 3
                            width: (parent.width - 28) / 2
                            onClicked: function(id) { root.organizationClicked(id) }
                        }
                    }
                }
            }
        }

        // Sidebar
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: root.rightPanelWidth
            color: theme.sidebarBg

            ColumnLayout {
                anchors.fill: parent
                anchors.topMargin: root.height * 0.08
                spacing: root.height * 0.02

                SidebarItem {
                    text: "Profile"
                    onClicked: root.profileRequested()
                }
                SidebarItem {
                    text: "Search"
                    active: true
                }
                SidebarItem {
                    text: "Organization"
                    onClicked: root.organizationClicked(0)
                }

                Image {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.preferredWidth: root.width * 0.25
                    Layout.preferredHeight: root.height * 0.7
                    Layout.topMargin: -root.height * 0.05
                    source: "../resources/lilac.png"
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                }

                Item { Layout.fillHeight: true }
            }
        }
    }

    component SidebarItem : Rectangle {
        property string text: ""
        property bool active: false
        signal clicked()

        Layout.fillWidth: true
        height: root.height * 0.07
        radius: 8
        color: active ? theme.purple : "transparent"

        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: root.width * 0.02
            text: parent.text
            font.family: theme.fontName
            font.pixelSize: root.height * 0.025
            font.bold: parent.active
            color: parent.active ? "white" : theme.textDark
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: if (!parent.active) parent.color = theme.accentPink
            onExited: if (!parent.active) parent.color = "transparent"
            onClicked: parent.clicked()
        }
    }
}
