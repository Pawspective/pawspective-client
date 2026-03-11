import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    anchors.fill: parent
    color: "#e8d8cb"

    signal registerRequested()
    signal loginSuccess()

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

    Connections {
        target: loginViewModel
        function onLoginFinished(success) {
            if (success) {
                root.loginSuccess()
            }
        }
        function onErrorOccurred(type, message) {
            errorMessageLabel.text = message
        }
    }

    property bool loading: false
    property string errorMessage: ""

    Rectangle {
        id: loginCard
        width: 480
        height: 600
        radius: 20
        color: theme.bgMain
        anchors.centerIn: parent


        ColumnLayout {
            anchors.centerIn: parent
            width: parent.width * 0.8
            spacing: 12

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 120
                
                Label {
                    text: "Login"
                    font.family: theme.fontName
                    font.pixelSize: 36
                    color: theme.textMain
                    anchors.centerIn: parent
                }

                Image {
                    id: catIcon
                    source: "../resources/tricky_cat.png"
                    fillMode: Image.PreserveAspectFit
                    width: 120
                    height: 120
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: -25
                    z: 10
                }
            }

            TextField {
                id: emailField
                placeholderText: "Email"
                text: loginViewModel.email
                onTextChanged: loginViewModel.email = text
                font.pointSize : 13
                placeholderTextColor: theme.accentPink
                color: theme.accentPink
                font.family: theme.fontName
                Layout.fillWidth: true
                leftPadding: 15
                background: Rectangle {
                    color: theme.bgInput
                    radius: 8
                }
            } 

            TextField {
                id: passwordField
                height: 60
                placeholderText: "Password"
                text: loginViewModel.password
                onTextChanged: loginViewModel.password = text
                font.pointSize : 13
                placeholderTextColor: theme.accentPink
                color: theme.accentPink
                font.family: theme.fontName
                echoMode: TextInput.Password
                Layout.fillWidth: true
                leftPadding: 15

                background: Rectangle {
                    color: theme.bgInput
                    radius: 8
                }
            }

            CustomButton {
                id: loginButton 
                text: loginViewModel.isBusy ? "Logging in..." : "Login"
                baseColor: theme.accentYellow
                hoverColor: theme.accentPink
                textColor: theme.textButton
                Layout.fillWidth: true
                enabled: !loginViewModel.isBusy
                
                onClicked: {
                    errorMessageLabel.text = ""
                    loginViewModel.login()
                } 
            }

            CustomButton {
                id: registerButton
                text: "Register"
                baseColor: theme.accentYellow
                hoverColor: theme.accentPink
                textColor: theme.textButton
                Layout.fillWidth: true
                enabled: !loginViewModel.isBusy
                onClicked: root.registerRequested()
            }

            LoaderSpinner {
                running: loginViewModel.isBusy
                visible: loginViewModel.isBusy
                Layout.alignment: Qt.AlignHCenter
            }

            Label {
                id: errorMessageLabel
                text: loginViewModel.errorMessage
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