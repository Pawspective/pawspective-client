import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    anchors.fill: parent
    color: "#e8d8cb"

    QtObject {
        id: theme
        readonly property string fontName: "Comic Sans MS"
        readonly property color bgMain: "#b2bb7d"
        readonly property color bgInput: "#fdfdfd"
        readonly property color accentPink: "#f4a7b9"
        readonly property color accentYellow: "#ede8b0"
        readonly property color textMain: "#f1e9bb"
        readonly property color textError: "#6c63ff"
        readonly property color textButton: "#b2bb7d"
    }

    property bool loading: false
    property string errorMessage: ""

    signal backClicked()
    signal registerSuccess()

    Rectangle {
        id: card
        width: 480
        height: 560
        radius: 20
        color: theme.bgMain
        anchors.centerIn: parent

        Text {
            text: "←"
            font.pixelSize: 30
            color: theme.textMain
            anchors {
                top: parent.top
                left: parent.left
                margins: 15
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: parent.color = theme.accentPink
                onExited: parent.color = theme.textMain
                onClicked: root.backClicked()
            }

            z: 10
        }

        ColumnLayout {
            anchors.centerIn: parent
            width: parent.width * 0.8
            spacing: 12

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 80

                Label {
                    text: "Register Organization"
                    font.family: theme.fontName
                    font.pixelSize: 26
                    color: theme.textMain
                    anchors.centerIn: parent
                }
            }

            TextField {
                id: nameField
                placeholderText: "Organization Name"
                placeholderTextColor: theme.accentPink
                color: theme.accentPink
                font.family: theme.fontName
                Layout.fillWidth: true
                leftPadding: 15
                enabled: !root.loading
                background: Rectangle { color: theme.bgInput; radius: 8 }
            }

            TextField {
                id: descriptionField
                placeholderText: "Description"
                placeholderTextColor: theme.accentPink
                color: theme.accentPink
                font.family: theme.fontName
                Layout.fillWidth: true
                leftPadding: 15
                enabled: !root.loading
                background: Rectangle { color: theme.bgInput; radius: 8 }
            }

            TextField {
                id: websiteField
                placeholderText: "Website (optional)"
                placeholderTextColor: theme.accentPink
                color: theme.accentPink
                font.family: theme.fontName
                Layout.fillWidth: true
                leftPadding: 15
                enabled: !root.loading
                background: Rectangle { color: theme.bgInput; radius: 8 }
            }

            CustomButton {
                id: submitBtn
                text: root.loading ? "Registering..." : "Register"
                baseColor: theme.accentYellow
                hoverColor: theme.accentPink
                textColor: theme.textButton
                Layout.fillWidth: true
                enabled: !root.loading
                onClicked: {
                    root.errorMessage = ""
                    // TODO: connect to registerOrganizationViewModel
                }
            }

            LoaderSpinner {
                running: root.loading
                visible: root.loading
                Layout.alignment: Qt.AlignHCenter
            }

            Label {
                id: errorMessageLabel
                text: root.errorMessage
                color: theme.textError
                font.family: theme.fontName
                visible: text.length > 0
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }
        }
    }
}
