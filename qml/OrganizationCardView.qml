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

    readonly property real pad: root.width * 0.07
    readonly property real avatarSize: root.width * 0.18
    readonly property real titleSize: root.width * 0.09
    readonly property real subtitleSize: root.width * 0.07
    readonly property real descSize: root.width * 0.065

    radius: root.width * 0.04
    color: "#fdfdfd"
    border.color: "#b8abd7"
    border.width: 1

    implicitHeight: contentLayout.implicitHeight + pad * 2

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onEntered: root.color = "#f3effa"
        onExited: root.color = "#fdfdfd"
        onClicked: root.clicked(root.organizationId)
    }

    ColumnLayout {
        id: contentLayout
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: root.pad
        }
        spacing: root.width * 0.04

        RowLayout {
            Layout.fillWidth: true
            spacing: root.width * 0.05

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
                    font.pixelSize: parent.width * 0.4
                    font.bold: true
                    color: "#8572af"
                }
            }

            ColumnLayout {
                spacing: root.width * 0.02
                Layout.fillWidth: true

                Text {
                    text: root.organizationName
                    font.family: "Comic Sans MS"
                    font.pixelSize: root.titleSize
                    font.bold: true
                    color: "#8572af"
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }

                Text {
                    text: root.organizationCity
                    font.family: "Comic Sans MS"
                    font.pixelSize: root.subtitleSize
                    color: "#f4a7b9"
                    elide: Text.ElideRight
                    Layout.fillWidth: true
                }
            }
        }

        Text {
            visible: root.organizationDescription.length > 0
            text: root.organizationDescription
            font.family: "Comic Sans MS"
            font.pixelSize: root.descSize
            color: "#8572af"
            wrapMode: Text.WordWrap
            maximumLineCount: 2
            elide: Text.ElideRight
            Layout.fillWidth: true
            opacity: 0.8
        }
    }
}
