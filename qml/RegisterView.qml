import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    anchors.fill: parent
    color: "#d6dec1"

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

    Timer {
        id: registerTimer
        interval: 2000
        onTriggered: {
            root.loading = false
            root.errorMessage = "Registration failed"
        }
    }
    Rectangle {
        id: loginCard
        width: 400
        height: 500
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
                Layout.preferredHeight: 120
                
                Label {
                    text: "Register"
                    font.family: theme.fontName
                    font.pixelSize: 32
                    color: theme.textMain
                    anchors.centerIn: parent
                }

                Image {
                    id: catIcon
                    source: "../resources/tricky_cat.png"
                    fillMode: Image.PreserveAspectFit
                    width: 120; height: 120
                    anchors {
                        right: parent.right
                        bottom: parent.bottom
                        bottomMargin: -25
                    }
                    z: 2 
                }
            }

            TextField {
                id: nameField
                placeholderText: "First Name"
                placeholderTextColor: theme.accentPink
                color: theme.accentPink
                font.family: theme.fontName
                Layout.fillWidth: true
                leftPadding: 15
                background: Rectangle { color: theme.bgInput; radius: 8 }
            } 

            TextField {
                id: surnameField
                placeholderText: "Last Name"
                placeholderTextColor: theme.accentPink
                color: theme.accentPink
                font.family: theme.fontName
                Layout.fillWidth: true
                leftPadding: 15
                background: Rectangle { color: theme.bgInput; radius: 8 }
            }

            TextField {
                id: emailField
                placeholderText: "Email"
                placeholderTextColor: theme.accentPink
                color: theme.accentPink
                font.family: theme.fontName
                Layout.fillWidth: true
                leftPadding: 15
                background: Rectangle { color: theme.bgInput; radius: 8 }
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
                background: Rectangle { color: theme.bgInput; radius: 8 }
            }

            TextField {
                id: confirmPasswordField
                placeholderText: "Confirm Password"
                placeholderTextColor: theme.accentPink
                color: theme.accentPink
                font.family: theme.fontName
                echoMode: TextInput.Password
                Layout.fillWidth: true
                leftPadding: 15
                background: Rectangle { color: theme.bgInput; radius: 8 }
            }

            CustomButton {
                id: registerBtn
                text: loading ? "Registering..." : "Register"
                baseColor: theme.accentYellow
                hoverColor: theme.accentPink
                textColor: theme.textButton
                Layout.fillWidth: true
                enabled: !loading
                
                onClicked: {
                    root.loading = true
                    root.errorMessage = ""
                    registerTimer.start()
                }
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