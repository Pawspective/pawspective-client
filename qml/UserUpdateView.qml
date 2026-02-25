import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

Rectangle {
    id: root
    anchors.fill: parent

    QtObject {
        id: theme
        readonly property string fontName: "Comic Sans MS"
        readonly property color pageBg: "#e8d8cb"
        readonly property color purple: "#b8abd7"
        readonly property color fieldBg: "#fdfdfd"
        readonly property color accentPink: "#f4a7b9"
        readonly property color textDark: "#8572af"
        readonly property color buttonText: "#e7ebf5"
    }

    property string userEmail: "email@example.com"
    property string userFirstName: "Alice"
    property string userLastName: "Brown"

    property bool loading: false
    property string errorMessage: ""

    signal submit()
    signal discard()

    color: theme.pageBg

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: theme.pageBg

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 40
                spacing: 25

                Text {
                    text: "Edit Profile"
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
                
                // Password field (special case)
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "Password (leave blank if don't want to change)"
                        font.family: theme.fontName
                        font.pixelSize: 18
                        color: theme.textDark
                    }

                    TextField {
                        id: passwordField
                        placeholderText: "Enter new password"
                        placeholderTextColor: theme.accentPink
                        color: theme.accentPink
                        font.family: theme.fontName
                        font.pixelSize: 18
                        echoMode: TextInput.Password
                        Layout.fillWidth: true
                        leftPadding: 15
                        enabled: !root.loading

                        background: Rectangle {
                            color: theme.fieldBg
                            radius: 10
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 15
                    Layout.topMargin: 10

                    CustomButton {
                        text: loading ? "Saving..." : "Save Changes"
                        baseColor: theme.purple
                        hoverColor: theme.accentPink
                        textColor: theme.buttonText
                        Layout.fillWidth: true
                        enabled: !root.loading

                        onClicked: {
                            root.loading = true
                            root.errorMessage = ""

                            var timer = Qt.createQmlObject('
                                import QtQuick 2.0;
                                Timer {
                                    interval: 2000;
                                    running: true;
                                    repeat: false;
                                    onTriggered: {
                                        root.loading = false;
                                        root.errorMessage = "Failed to update profile";
                                        root.submit();
                                    }
                                }
                            ', root)
                            timer.start()
                        }
                    }

                    CustomButton {
                        text: "Discard Changes"
                        baseColor: theme.purple
                        hoverColor: theme.accentPink
                        textColor: theme.buttonText
                        Layout.fillWidth: true
                        enabled: !root.loading

                        onClicked: {
                            root.discard()
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 100
                    Layout.alignment: Qt.AlignHCenter
                    visible: root.loading

                    LoaderSpinner {
                        anchors.centerIn: parent
                        running: root.loading
                        visible: root.loading
                        scale: 2.0
                    }
                }

                Label {
                    text: root.errorMessage
                    color: "#6c63ff"
                    font.family: theme.fontName
                    visible: text.length > 0
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
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

        TextField {
            text: parent.value
            placeholderText: parent.label
            placeholderTextColor: theme.accentPink
            color: theme.accentPink
            font.family: theme.fontName
            font.pixelSize: 18
            Layout.fillWidth: true
            leftPadding: 15
            selectByMouse: true
            enabled: !root.loading

            background: Rectangle {
                color: theme.fieldBg
                radius: 10
            }

            onTextChanged: {
                if (parent.label === "Email") root.userEmail = text
                else if (parent.label === "First Name") root.userFirstName = text
                else if (parent.label === "Last Name") root.userLastName = text
            }
        }
    }
}