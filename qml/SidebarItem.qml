import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: sidebarItemRoot
    
    property string text: ""
    property bool active: false
    
    property string fontName: "Arial"
    property real sidebarItemFontSize: 14
    property real sidebarItemLeftMargin: 10
    property color purple: "#b8abd7"
    property color textDark: "#8572af"
    property color accentPink: "#f4a7b9"

    Layout.fillWidth: true
    radius: 8
    color: active ? purple : "transparent"

    signal clicked()

    Text {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: sidebarItemRoot.sidebarItemLeftMargin
        text: sidebarItemRoot.text || ""
        font.family: sidebarItemRoot.fontName
        font.pixelSize: sidebarItemRoot.sidebarItemFontSize
        font.bold: sidebarItemRoot.active
        color: sidebarItemRoot.active ? "white" : sidebarItemRoot.textDark
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: if (!sidebarItemRoot.active) sidebarItemRoot.color = sidebarItemRoot.accentPink
        onExited: if (!sidebarItemRoot.active) sidebarItemRoot.color = "transparent"
        onClicked: sidebarItemRoot.clicked()
    }
}