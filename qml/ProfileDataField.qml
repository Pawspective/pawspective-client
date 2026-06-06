import QtQuick 2.15
import QtQuick.Layouts 1.15

ColumnLayout {
    id: fieldRoot
    
    property string label: ""
    property string value: ""
    
    property string fontName: "Arial"
    property real fieldLabelFontSize: 14
    property real fieldValueFontSize: 16
    property real fieldHeight: 40
    property real fieldLeftMargin: 10
    property color textDark: "#000000"
    property color fieldBg: "#ffffff"
    property color accentPink: "#ff0000"

    Layout.fillWidth: true

    Text {
        text: fieldRoot.label || ""
        font.family: fieldRoot.fontName
        font.pixelSize: fieldRoot.fieldLabelFontSize
        color: fieldRoot.textDark
    }

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: fieldRoot.fieldHeight
        radius: 10
        color: fieldRoot.fieldBg

        Text {
            anchors.left: parent.left
            anchors.leftMargin: fieldRoot.fieldLeftMargin
            anchors.verticalCenter: parent.verticalCenter
            text: fieldRoot.value || ""
            font.family: fieldRoot.fontName
            font.pixelSize: fieldRoot.fieldValueFontSize
            color: fieldRoot.accentPink
        }
    }
}