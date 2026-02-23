import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

Rectangle {
    id: root
    anchors.fill: parent

    readonly property color pageColor: "#e8d8cb"
    readonly property color purpleColor: "#8572af"
    readonly property color fieldColor: "#e7ebf5"
    readonly property color buttonColor: "#b8abd7"
    readonly property color hoverColor: "#f4a7b9"
    readonly property color textColor: "#f4a7b9"

    property string userEmail: "email@example.com"
    property string userFirstName: "Alice"
    property string userLastName: "Brown"

    property bool loading: false
    property string errorMessage: ""

    signal submit()
    signal discard()

    color: pageColor

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: pageColor

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 40
                spacing: 25

                // Заголовок
                Label {
                    text: "Edit Profile"
                    font.family: "Comic Sans MS"
                    font.pixelSize: 32
                    font.bold: true
                    color: purpleColor
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: 20
                }

                // Email
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "Email"
                        font.family: "Comic Sans MS"
                        font.pixelSize: 18
                        color: purpleColor
                    }

                    TextField {
                        id: emailField
                        text: root.userEmail
                        placeholderText: "Email"
                        placeholderTextColor: textColor
                        color: textColor
                        font.family: "Comic Sans MS"
                        font.pixelSize: 18
                        Layout.fillWidth: true
                        leftPadding: 15
                        selectByMouse: true
                        enabled: !root.loading

                        background: Rectangle {
                            color: fieldColor
                            radius: 10
                        }
                    }
                }

                // First Name
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "First Name"
                        font.family: "Comic Sans MS"
                        font.pixelSize: 18
                        color: purpleColor
                    }

                    TextField {
                        id: firstNameField
                        text: root.userFirstName
                        placeholderText: "First Name"
                        placeholderTextColor: textColor
                        color: textColor
                        font.family: "Comic Sans MS"
                        font.pixelSize: 18
                        Layout.fillWidth: true
                        leftPadding: 15
                        selectByMouse: true
                        enabled: !root.loading

                        background: Rectangle {
                            color: fieldColor
                            radius: 10
                        }
                    }
                }

                // Last Name
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "Last Name"
                        font.family: "Comic Sans MS"
                        font.pixelSize: 18
                        color: purpleColor
                    }

                    TextField {
                        id: lastNameField
                        text: root.userLastName
                        placeholderText: "Last Name"
                        placeholderTextColor: textColor
                        color: textColor
                        font.family: "Comic Sans MS"
                        font.pixelSize: 18
                        Layout.fillWidth: true
                        leftPadding: 15
                        selectByMouse: true
                        enabled: !root.loading

                        background: Rectangle {
                            color: fieldColor
                            radius: 10
                        }
                    }
                }

                // Password
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "Password"
                        font.family: "Comic Sans MS"
                        font.pixelSize: 18
                        color: purpleColor
                    }

                    TextField {
                        id: passwordField
                        placeholderText: "Enter new password"
                        placeholderTextColor: textColor
                        color: textColor
                        font.family: "Comic Sans MS"
                        font.pixelSize: 18
                        echoMode: TextInput.Password
                        Layout.fillWidth: true
                        leftPadding: 15
                        enabled: !root.loading

                        background: Rectangle {
                            color: fieldColor
                            radius: 10
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 15
                    Layout.topMargin: 10

                    Button {
                        text: loading ? "Saving..." : "Save Changes"
                        Layout.fillWidth: true
                        Layout.preferredHeight: 50
                        enabled: !root.loading

                        contentItem: Text {
                            text: parent.text
                            font.family: "Comic Sans MS"
                            font.pixelSize: 18
                            color: "#e7ebf5"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            color: parent.pressed ? "black" : buttonColor
                            radius: 8
                        }

                        onClicked: {
                            root.loading = true
                            root.errorMessage = ""

                            Qt.createQmlObject("
                                import QtQuick 2.0;
                                Timer {
                                    interval: 2000;
                                    running: true;
                                    repeat: false;
                                    onTriggered: {
                                        root.loading = false;
                                        root.errorMessage = 'Failed to update profile';
                                        root.submit();
                                    }
                                }
                            ", root)
                        }
                    }

                    Button {
                        text: "Discard Changes"
                        Layout.fillWidth: true
                        Layout.preferredHeight: 50
                        enabled: !root.loading

                        contentItem: Text {
                            text: parent.text
                            font.family: "Comic Sans MS"
                            font.pixelSize: 18
                            color: "#e7ebf5"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            color: parent.pressed ? "black" : hoverColor
                            radius: 8
                        }

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

                // Error message
                Label {
                    text: root.errorMessage
                    color: "#6c63ff"
                    font.family: "Comic Sans MS"
                    visible: text.length > 0
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                }

                Item {
                    Layout.fillHeight: true
                }
            }
        }
    }
}