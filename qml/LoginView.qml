import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    width: 400
    height: 500
    color: "#b2bb7d"
    readonly property string customFont: "Comic Sans MS"
    property bool loading: false
    property string errorMessage: ""

    ColumnLayout {
        anchors.centerIn: parent
        width: parent.width * 0.8
        spacing: 12

         Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            
            Label {
                text: "Login"
                font.family: customFont
                font.pixelSize: 32
                color: "#f1e9bb"
                anchors.centerIn: parent
            }

            Image {
                id: catIcon
                source: "assets/tricky_cat.png"
                fillMode: Image.PreserveAspectFit
                width: 120
                height: 120
                
                anchors.right: parent.right
                anchors.rightMargin: 0
                
                anchors.bottom: parent.bottom
                anchors.bottomMargin: -25
                
                z: 2 
            }
        }

        TextField {
            id: emailField
            placeholderText: "Email"
            placeholderTextColor: "#f4a7b9"
            color: "#f4a7b9"
            font.family: customFont
            Layout.fillWidth: true
            leftPadding: 15
            
            background: Rectangle {
                color: "#fdfdfd"
                radius: 8
            }
        } 

        TextField {
            id: passwordField
            placeholderText: "Password"
            placeholderTextColor: "#f4a7b9"
            color: "#f4a7b9"
            font.family: customFont
            echoMode: TextInput.Password
            Layout.fillWidth: true
            leftPadding: 15

            background: Rectangle {
                color: "#fdfdfd"
                radius: 8
            }
        }

        Button {
            text: loading ? "Logging in..." : "Login"
            contentItem: Text {
                text: parent.text
                font.family: customFont
                font.pixelSize: 18
                color: "#b2bb7d"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }

            background: Rectangle {
                color: parent.pressed ? "black" : "#ede8b0"
                radius: 5
                border.color: "#f1e9bb"
                border.width: 1
            }

            Layout.fillWidth: true

            enabled: !loading

            onClicked: {

                root.loading = true
                root.errorMessage = ""

                // simulate request
                Qt.createQmlObject("
                    import QtQuick 2.0;
                    Timer {
                        interval: 2000;
                        running: true;
                        repeat: false;
                        onTriggered: {
                            root.loading = false;
                            root.errorMessage = 'Invalid email or password';
                        }
                    }
                ", root)
            }
        }

        Button {
            id: regBtn
            text: "Register"
            Layout.fillWidth: true
            Layout.preferredHeight: 40

            contentItem: Text {
                text: regBtn.text
                font.family: customFont
                font.pixelSize: 18
                color: "#b2bb7d"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle {
                color: parent.pressed ? "black" : "#ede8b0"
                radius: 5
                border.color: "#f1e9bb"
                border.width: 1
            }
        }

        LoaderSpinner {
            running: root.loading
            visible: root.loading
            Layout.alignment: Qt.AlignHCenter
        }

        Label {
            text: root.errorMessage
            color: "#6c63ff"
            font.family: customFont
            visible: text.length > 0
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
    }
}
