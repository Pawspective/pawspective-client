import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    anchors.fill: parent

    property var viewModel: null

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

    readonly property real leftPanelWidth: root.width * 0.7
    readonly property real rightPanelWidth: root.width * 0.3
    readonly property real contentMargins: root.height * 0.05
    readonly property real contentSpacing: root.height * 0.025
    readonly property real titleFontSize: root.height * 0.05
    readonly property real titleHeight: root.height * 0.08
    readonly property real fieldLabelFontSize: root.height * 0.025
    readonly property real fieldValueFontSize: root.height * 0.03
    readonly property real fieldHeight: root.height * 0.07
    readonly property real fieldLeftMargin: root.width * 0.01
    readonly property real fieldSpacing: root.height * 0.01
    readonly property real buttonWidth: root.width * 0.25
    readonly property real buttonHeight: root.height * 0.08
    readonly property real buttonSpacing: root.height * 0.02
    readonly property real buttonRowSpacing: root.width * 0.02
    readonly property real catSize: root.height * 0.35
    readonly property real sidebarTopMargin: root.height * 0.08
    readonly property real sidebarSpacing: root.height * 0.02
    readonly property real sidebarItemHeight: root.height * 0.07
    readonly property real sidebarItemFontSize: root.height * 0.025
    readonly property real sidebarItemLeftMargin: root.width * 0.02
    readonly property real sidebarImageWidth: root.width * 0.25
    readonly property real sidebarImageHeight: root.height * 0.7
    readonly property real sidebarImageTopMargin: -root.height * 0.05

    signal logoutClicked()
    signal editProfileClicked()

    color: theme.pageBg

    LoaderSpinner {
        anchors.centerIn: parent
        running: viewModel ? viewModel.isBusy : false
        visible: viewModel ? viewModel.isBusy : false
        z: 10
    }

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
                anchors.margins: root.contentMargins
                spacing: root.contentSpacing

                Text {
                    text: "Profile"
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.titleHeight
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.family: theme.fontName
                    font.pixelSize: root.titleFontSize
                    font.bold: true
                    color: theme.textDark
                }

                ProfileDataField { 
                    label: "Email"
                    value: viewModel ? viewModel.userData.email : ""
                }
                
                ProfileDataField { 
                    label: "First Name"
                    value: viewModel ? viewModel.userData.firstName : ""
                }
                
                ProfileDataField { 
                    label: "Last Name"
                    value: viewModel ? viewModel.userData.lastName : ""
                }

                ProfileDataField { 
                    label: "Organization ID"
                    value: viewModel && viewModel.userData.organizationId ? 
                           viewModel.userData.organizationId : "None"
                    visible: viewModel && viewModel.userData.organizationId.has_value
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: root.buttonRowSpacing

                    ColumnLayout {
                        spacing: root.buttonSpacing
                        Layout.alignment: Qt.AlignLeft

                        CustomButton {
                            text: "Edit Profile"
                            baseColor: theme.purple
                            hoverColor: theme.accentPink
                            textColor: theme.buttonText
                            fontSize: Math.min(root.height * 0.035, root.width * 0.018)
                            Layout.preferredWidth: root.buttonWidth
                            Layout.preferredHeight: root.buttonHeight
                            onClicked: root.editProfileClicked()
                        }

                        CustomButton {
                            text: "Logout"
                            baseColor: theme.purple
                            hoverColor: theme.accentPink
                            textColor: theme.buttonText
                            fontSize: Math.min(root.height * 0.035, root.width * 0.018)
                            Layout.preferredWidth: root.buttonWidth
                            Layout.preferredHeight: root.buttonHeight
                            onClicked: root.logoutClicked()
                        }
                    }

                    Item { Layout.fillWidth: true }

                    Image {
                        Layout.preferredWidth: root.catSize
                        Layout.preferredHeight: root.catSize
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
                anchors.topMargin: root.sidebarTopMargin
                spacing: root.sidebarSpacing

                SidebarItem { text: "Profile"; active: true }
                SidebarItem { text: "Search" }
                SidebarItem { text: "Organization" }

                Image {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.preferredWidth: root.sidebarImageWidth
                    Layout.preferredHeight: root.sidebarImageHeight
                    Layout.topMargin: root.sidebarImageTopMargin
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
        spacing: root.fieldSpacing

        Text {
            text: parent.label
            font.family: theme.fontName
            font.pixelSize: root.fieldLabelFontSize
            color: theme.textDark
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: root.fieldHeight
            radius: 10
            color: theme.fieldBg

            Text {
                anchors.left: parent.left
                anchors.leftMargin: root.fieldLeftMargin
                anchors.verticalCenter: parent.verticalCenter
                text: parent.parent.value
                font.family: theme.fontName
                font.pixelSize: root.fieldValueFontSize
                color: theme.accentPink
            }
        }
    }

    component SidebarItem : Rectangle {
        property string text: ""
        property bool active: false
        Layout.fillWidth: true
        height: root.sidebarItemHeight
        radius: 8
        color: active ? theme.purple : "transparent"

        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: root.sidebarItemLeftMargin
            text: parent.text
            font.family: theme.fontName
            font.pixelSize: root.sidebarItemFontSize
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