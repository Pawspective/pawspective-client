import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property string organizationName: ""
    property string organizationCity: ""
    property string organizationDescription: ""
    property int organizationId: 0

    signal clicked(int organizationId)

    readonly property real padV: root.width * 0.025
    readonly property real padH: root.width * 0.06
    readonly property real avatarSize: root.width * 0.09
    readonly property real titleSize: root.width * 0.028
    readonly property real subtitleSize: root.width * 0.022
    readonly property real descSize: root.width * 0.016

    radius: root.width * 0.015
    color: "#b8abd7"

    implicitHeight: contentLayout.implicitHeight + padV * 2

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onEntered: root.color = "#f4a7b9"
        onExited: root.color = "#b8abd7"
        onClicked: root.clicked(root.organizationId)
    }

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
                border.color: "#b8abd7"
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: root.organizationName.length > 0 ? root.organizationName[0].toUpperCase() : "?"
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
                    text: root.organizationName
                    font.family: "Comic Sans MS"
                    font.pixelSize: root.titleSize
                    font.bold: true
                    color: "#fdfdfd"
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                Text {
                    text: root.organizationCity
                    font.family: "Comic Sans MS"
                    font.pixelSize: root.subtitleSize
                    color: "#e8d8cb"
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
            }
        }

        Text {
            visible: root.organizationDescription.length > 0
            text: "About us: " + root.organizationDescription
            font.family: "Comic Sans MS"
            font.pixelSize: root.descSize
            color: "#fdfdfd"
            wrapMode: Text.WordWrap
            maximumLineCount: 2
            elide: Text.ElideRight
            Layout.fillWidth: true
            opacity: 0.8
        }
    }
}
