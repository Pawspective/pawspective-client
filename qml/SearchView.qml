import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    color: "#e8d8cb"

    signal profileRequested()
    signal organizationClicked(int organizationId)
    signal animalDetailRequested(int animalId)

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
                    contentWidth: -1
                    ScrollBar.vertical.policy: ScrollBar.AlwaysOn

                    ColumnLayout {
                        width: cardsScroll.width - cardsScroll.ScrollBar.vertical.width
                        spacing: 8

                        // TODO: mock — replace with Repeater over search results
                        AnimalCardView {
                            animalName: "Бублик"
                            animalType: "Dog"
                            animalAge: 3
                            animalDescription: "Из твоей организации"
                            animalId: 2
                            Layout.fillWidth: true
                            onClicked: function(id) { root.animalDetailRequested(id) }
                        }

                        AnimalCardView {
                            animalName: "Мурзик"
                            animalType: "Cat"
                            animalAge: 5
                            animalDescription: "Из другой организации"
                            animalId: 3
                            Layout.fillWidth: true
                            onClicked: function(id) { root.animalDetailRequested(id) }
                        }

                        // TODO: mock — replace with Repeater over search results
                        OrganizationCardView {
                            organizationName: "LOL"
                            organizationCity: "Moscow"
                            organizationDescription: "Дырбулщищ"
                            organizationId: 1
                            Layout.fillWidth: true
                            onClicked: function(id) { root.organizationClicked(id) }
                        }

                        OrganizationCardView {
                            organizationName: "KEK"
                            organizationCity: "Тульская губерния"
                            organizationDescription: "На краю дороги стоял дуб. Вероятно, в десять раз старше берёз, составлявших лес, он был в десять раз толще и в два раза выше каждой берёзы. Это был огромный в два обхвата дуб с обломанными, давно видно, суками и с обломанной корой, заросшей старыми болячками. С огромными своими неуклюжими, несимметрично-растопыренными, корявыми руками и пальцами, он старым, сердитым и презрительным уродом стоял между улыбающимися берёзами. Только он один не хотел подчиняться обаянию весны и не хотел видеть ни весны, ни солнца."
                            organizationId: 3
                            Layout.fillWidth: true
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
