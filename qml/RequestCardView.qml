import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property qint64 requestId: -1
    property string requestEmail: ""
    property string animalName: ""
    property string animalBreed: ""
    property int animalAge: 0
    property string animalDescription: ""
    property bool canActOnRequest: false

    signal acceptClicked(int requestId)
    signal denyClicked(int requestId)

    readonly property real padV: root.width * 0.025
    readonly property real padH: root.width * 0.06
    readonly property real avatarSize: root.width * 0.09
    readonly property real titleSize: root.width * 0.028
    readonly property real subtitleSize: root.width * 0.022
    readonly property real descSize: root.width * 0.016

    radius: root.width * 0.015
    color: "#b8abd7"

    implicitHeight: contentLayout.implicitHeight + padV * 2

    ColumnLayout {
        id: contentLayout
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            leftMargin: root.padH
            rightMargin: root.padH
            topMargin: root.padV
        }
        spacing: root.padV * 0.5

        RowLayout {
            Layout.fillWidth: true
            spacing: root.padH

            Rectangle {
                width: root.avatarSize
                height: root.avatarSize
                radius: width / 2
                color: "#e8d8cb"
                border.color: "#8572af"
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: root.animalBreed.length > 0 ? root.animalBreed[0].toUpperCase() : "?"
                    font.family: "Comic Sans MS"
                    font.pixelSize: parent.width * 0.45
                    font.bold: true
                    color: "#8572af"
                }
            }

            ColumnLayout {
                spacing: root.padV * 0.2
                Layout.fillWidth: true

                Text {
                    text: root.animalName
                    font.family: "Comic Sans MS"
                    font.pixelSize: root.titleSize
                    font.bold: true
                    color: "#fdfdfd"
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                Text {
                    text: root.animalBreed + ", " + root.animalAge + " y.o."
                    font.family: "Comic Sans MS"
                    font.pixelSize: root.subtitleSize
                    color: "#e8d8cb"
                }

                Text {
                    text: root.requestEmail
                    font.family: "Comic Sans MS"
                    font.pixelSize: root.descSize
                    color: "#fdfdfd"
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                    opacity: 0.85
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

        RowLayout {
            visible: root.canActOnRequest
            Layout.fillWidth: true
            spacing: root.padH * 0.5
            Layout.bottomMargin: root.padV * 0.5

            Item { Layout.fillWidth: true }

            CustomButton {
                text: "Accept"
                baseColor: "#7caf7c"
                hoverColor: "#5d9e5d"
                clickColor: "#4a8a4a"
                textColor: "#fdfdfd"
                fontSize: root.descSize
                implicitWidth: root.width * 0.12
                implicitHeight: root.width * 0.045
                radius: root.width * 0.008
                onClicked: root.acceptClicked(root.requestId)
            }

            CustomButton {
                text: "Deny"
                baseColor: "#f4a7b9"
                hoverColor: "#e0809a"
                clickColor: "#cc5a70"
                textColor: "#fdfdfd"
                fontSize: root.descSize
                implicitWidth: root.width * 0.12
                implicitHeight: root.width * 0.045
                radius: root.width * 0.008
                onClicked: root.denyClicked(root.requestId)
            }
        }
    }
}
