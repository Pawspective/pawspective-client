import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

Rectangle {
    id: root
    anchors.fill: parent

    property var viewModel: null

    QtObject {
        id: theme
        readonly property string fontName: "Comic Sans MS"
        readonly property color pageBg: "#e8d8cb"
        readonly property color purple: "#b8abd7"
        readonly property color fieldBg: "#fdfdfd"
        readonly property color accentPink: "#f4a7b9"
        readonly property color textDark: "#8572af"
        readonly property color buttonText: "#e7ebf5"
        readonly property color errorColor: "#ff6b6b"
        readonly property color successColor: "#51cf66"
    }

    property string userEmail: viewModel ? viewModel.email : ""
    property string userFirstName: viewModel ? viewModel.firstName : ""
    property string userLastName: viewModel ? viewModel.lastName : ""
    property bool loading: viewModel ? viewModel.isBusy : false
    property string errorMessage: ""

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
    readonly property real fieldLeftMargin: root.width * 0.01

    signal submit()
    signal discard()

    color: theme.pageBg

    Connections {
        target: viewModel
        function onSaveFailed(message) {
            errorMessage = message
        }
        function onSessionExpired() {
            errorMessage = "Session expired. Please log in again."
            closeTimer.start()
        }
    }

    Timer {
        id: closeTimer
        interval: 2000
        onTriggered: root.discard()
    }

    Component.onCompleted: {
        if (viewModel) {
            viewModel.initialize()
        }
    }

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

                ProfileDataField {
                    label: "Email"
                    value: viewModel ? viewModel.email : ""
                    onValueChanged: if (viewModel) viewModel.email = value
                }

                ProfileDataField {
                    label: "First Name"
                    value: viewModel ? viewModel.firstName : ""
                    onValueChanged: if (viewModel) viewModel.firstName = value
                }

                ProfileDataField {
                    label: "Last Name"
                    value: viewModel ? viewModel.lastName : ""
                    onValueChanged: if (viewModel) viewModel.lastName = value
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: root.fieldSpacing
                    Layout.topMargin: root.fieldPasswordTopMargin

                    Text {
                        text: "New Password (leave blank if you don't want to change)"
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
                        text: viewModel ? viewModel.newPassword : ""
                        onTextChanged: if (viewModel) viewModel.newPassword = text

                        background: Rectangle {
                            color: theme.fieldBg
                            radius: 10
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: root.buttonSpacing
                    Layout.topMargin: root.contentSpacing

                    CustomButton {
                        text: root.loading ? "Saving..." : "Save Changes"
                        baseColor: (viewModel && viewModel.isDirty) ? theme.purple : "#cccccc"
                        hoverColor: (viewModel && viewModel.isDirty) ? theme.accentPink : "#cccccc"
                        textColor: theme.buttonText
                        fontSize: root.buttonFontSize
                        Layout.fillWidth: true
                        Layout.preferredHeight: root.buttonHeight
                        enabled: !root.loading && viewModel && viewModel.isDirty

                        onClicked: {
                            if (viewModel) viewModel.saveChanges()
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
                            if (viewModel) viewModel.discardChanges()
                            root.discard()
                        }
                    }
                }

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

                Label {
                    text: root.errorMessage
                    color: theme.errorColor
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
        signal valueChanged(string newValue)

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
                parent.valueChanged(text)
            }
        }
    }
}