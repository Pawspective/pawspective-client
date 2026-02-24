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

    property bool loading: false
    property string errorMessage: ""

    Timer {
        id: loginTimer
        interval: 2000
        onTriggered: {
            root.loading = false
            root.errorMessage = "Invalid email or password"
            root.loginSuccess()
        }
    }

    Rectangle {
        id: loginCard
        width: 400
        height: 500
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
                    font.pixelSize: 32
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
                placeholderText: "Password"
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
                text: loading ? "Logging in..." : "Login"
                baseColor: theme.accentYellow
                hoverColor: theme.accentPink
                textColor: theme.textButton
                Layout.fillWidth: true
                enabled: !loading
                
                onClicked: {
                    root.loading = true
                    root.errorMessage = ""
                    loginTimer.start()
                } 
            }

            CustomButton {
                id: registerButton
                text: "Register"
                baseColor: theme.accentYellow
                hoverColor: theme.accentPink
                textColor: theme.textButton
                Layout.fillWidth: true
                enabled: !loading
                onClicked: root.registerRequested()
            }

            LoaderSpinner {
                running: root.loading
                visible: root.loading
                Layout.alignment: Qt.AlignHCenter
            }

            Label {
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