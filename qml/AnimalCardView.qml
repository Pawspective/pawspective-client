import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property string animalName: ""
    property string animalType: ""
    property int animalAge: 0
<<<<<<< HEAD
    property string animalDescription: ""
=======
>>>>>>> e542a765a92e81fc781769114a70bcbd97b60e30
    property int animalId: 0

    signal clicked(int animalId)

<<<<<<< HEAD
    readonly property real pad: root.width * 0.025
    readonly property real avatarSize: root.width * 0.05
    readonly property real titleSize: root.width * 0.028
    readonly property real subtitleSize: root.width * 0.022
    readonly property real descSize: root.width * 0.020

    radius: root.width * 0.015
    color: "#b8abd7"
=======
    readonly property real pad: root.width * 0.07
    readonly property real avatarSize: root.width * 0.18
    readonly property real titleSize: root.width * 0.09
    readonly property real subtitleSize: root.width * 0.07

    radius: root.width * 0.04
    color: "#fdfdfd"
    border.color: "#b8abd7"
    border.width: 1
>>>>>>> e542a765a92e81fc781769114a70bcbd97b60e30

    implicitHeight: contentLayout.implicitHeight + pad * 2

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
<<<<<<< HEAD
        onEntered: root.color = "#f4a7b9"
        onExited: root.color = "#b8abd7"
=======
        onEntered: root.color = "#f3effa"
        onExited: root.color = "#fdfdfd"
>>>>>>> e542a765a92e81fc781769114a70bcbd97b60e30
        onClicked: root.clicked(root.animalId)
    }

    ColumnLayout {
        id: contentLayout
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: root.pad
        }
<<<<<<< HEAD
        spacing: root.pad * 0.5

        RowLayout {
            Layout.fillWidth: true
            spacing: root.pad
=======
        spacing: root.width * 0.04

        RowLayout {
            Layout.fillWidth: true
            spacing: root.width * 0.05
>>>>>>> e542a765a92e81fc781769114a70bcbd97b60e30

            Rectangle {
                width: root.avatarSize
                height: root.avatarSize
                radius: width / 2
                color: "#e8d8cb"
<<<<<<< HEAD
                border.color: "#8572af"
=======
                border.color: "#b8abd7"
>>>>>>> e542a765a92e81fc781769114a70bcbd97b60e30
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: root.animalType.length > 0 ? root.animalType[0].toUpperCase() : "?"
                    font.family: "Comic Sans MS"
<<<<<<< HEAD
                    font.pixelSize: parent.width * 0.45
=======
                    font.pixelSize: root.avatarSize * 0.45
>>>>>>> e542a765a92e81fc781769114a70bcbd97b60e30
                    font.bold: true
                    color: "#8572af"
                }
            }

            ColumnLayout {
<<<<<<< HEAD
                spacing: root.pad * 0.2
=======
                spacing: root.width * 0.02
>>>>>>> e542a765a92e81fc781769114a70bcbd97b60e30
                Layout.fillWidth: true

                Text {
                    text: root.animalName
                    font.family: "Comic Sans MS"
                    font.pixelSize: root.titleSize
                    font.bold: true
<<<<<<< HEAD
                    color: "#fdfdfd"
=======
                    color: "#8572af"
>>>>>>> e542a765a92e81fc781769114a70bcbd97b60e30
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                Text {
<<<<<<< HEAD
                    text: root.animalType + ", " + root.animalAge + " y.o."
                    font.family: "Comic Sans MS"
                    font.pixelSize: root.subtitleSize
                    color: "#e8d8cb"
                }
            }
        }

        Text {
            visible: root.animalDescription.length > 0
            text: root.animalDescription
            font.family: "Comic Sans MS"
            font.pixelSize: root.descSize
            color: "#fdfdfd"
            wrapMode: Text.WordWrap
            maximumLineCount: 2
            elide: Text.ElideRight
            Layout.fillWidth: true
            opacity: 0.9
        }
=======
                    text: root.animalType + " · " + root.animalAge + " y.o."
                    font.family: "Comic Sans MS"
                    font.pixelSize: root.subtitleSize
                    color: "#f4a7b9"
                }
            }
        }
>>>>>>> e542a765a92e81fc781769114a70bcbd97b60e30
    }
}
