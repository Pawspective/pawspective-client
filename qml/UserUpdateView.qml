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

    // Размеры в процентах
    readonly property real fieldLabelFontSize: root.height * 0.022
readonly property real fieldValueFontSize: root.height * 0.025
readonly property real fieldHeight: root.height * 0.06
readonly property real fieldSpacing: root.height * 0.008
readonly property real fieldPasswordTopMargin: root.height * 0.01
readonly property real buttonHeight: root.height * 0.07
readonly property real buttonSpacing: root.height * 0.015
readonly property real buttonFontSize: Math.min(root.height * 0.035, root.width * 0.04)
readonly property real titleFontSize: root.height * 0.045
readonly property real titleHeight: root.height * 0.07
readonly property real contentMargins: root.height * 0.04
readonly property real contentSpacing: root.height * 0.02
readonly property real loaderSize: root.height * 0.18
readonly property real loaderTopMargin: root.height * 0.02

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
                anchors.margins: root.contentMargins
                spacing: root.contentSpacing

                // Заголовок
                Text {
                    text: "Edit Profile"
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.titleHeight
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.family: theme.fontName
                    font.pixelSize: root.titleFontSize
                    font.bold: true
                    color: theme.textDark
                }

                // Поля данных
                ProfileDataField { 
                    label: "Email"
                    value: root.userEmail
                }
                
                ProfileDataField { 
                    label: "First Name"
                    value: root.userFirstName
                }
                
                ProfileDataField { 
                    label: "Last Name"
                    value: root.userLastName
                }
                
                // Password field (special case)
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: root.fieldSpacing
                    Layout.topMargin: root.fieldPasswordTopMargin

                    Text {
                        text: "Password (leave blank if don't want to change)"
                        font.family: theme.fontName
                        font.pixelSize: root.fieldLabelFontSize
                        color: theme.textDark
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }

                    TextField {
                        id: passwordField
                        placeholderText: "Enter new password"
                        placeholderTextColor: theme.accentPink
                        color: theme.accentPink
                        font.family: theme.fontName
                        font.pixelSize: root.fieldValueFontSize
                        echoMode: TextInput.Password
                        Layout.fillWidth: true
                        Layout.preferredHeight: root.fieldHeight
                        leftPadding: root.fieldLeftMargin
                        enabled: !root.loading

                        background: Rectangle {
                            color: theme.fieldBg
                            radius: 10
                        }
                    }
                }

                // Кнопки
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: root.buttonSpacing
                    Layout.topMargin: root.contentSpacing

                    CustomButton {
                        text: root.loading ? "Saving..." : "Save Changes"
                        baseColor: theme.purple
                        hoverColor: theme.accentPink
                        textColor: theme.buttonText
                        fontSize: root.buttonFontSize
                        Layout.fillWidth: true
                        Layout.preferredHeight: root.buttonHeight
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
                        fontSize: root.buttonFontSize
                        Layout.fillWidth: true
                        Layout.preferredHeight: root.buttonHeight
                        enabled: !root.loading

                        onClicked: {
                            root.discard()
                        }
                    }
                }

                // Лоадер
                LoaderSpinner {
    Layout.fillWidth: true
    Layout.preferredHeight: root.loaderSize
    Layout.maximumHeight: root.loaderSize
    Layout.minimumHeight: root.loaderSize
    Layout.alignment: Qt.AlignHCenter
    Layout.topMargin: root.loaderTopMargin
    Layout.bottomMargin: root.contentSpacing
    running: root.loading
    visible: root.loading
}

                // Error message
                Label {
                    text: root.errorMessage
                    color: "#6c63ff"
                    font.family: theme.fontName
                    font.pixelSize: root.fieldLabelFontSize
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
        spacing: root.fieldSpacing

        Text {
            text: parent.label
            font.family: theme.fontName
            font.pixelSize: root.fieldLabelFontSize
            color: theme.textDark
        }

        TextField {
            text: parent.value
            placeholderText: parent.label
            placeholderTextColor: theme.accentPink
            color: theme.accentPink
            font.family: theme.fontName
            font.pixelSize: root.fieldValueFontSize
            Layout.fillWidth: true
            Layout.preferredHeight: root.fieldHeight
            leftPadding: root.fieldLeftMargin
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