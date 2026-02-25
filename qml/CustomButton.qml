import QtQuick 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property string text: "Button"
    property color baseColor: "#ede8b0"
    property color hoverColor: "#e0d990"
    property color clickColor: "#fdfdfd" 
    property color textColor: "#b2bb7d"
    property int fontSize: 18

    signal clicked()

    implicitWidth: 250
    implicitHeight: 60
    radius: 14
    
    color: mouseArea.pressed ? clickColor : (mouseArea.containsMouse ? hoverColor : baseColor)

    Behavior on color {
        ColorAnimation { duration: 150 }
    }

    scale: mouseArea.pressed ? 0.97 : 1.0
    Behavior on scale {
        NumberAnimation { duration: 50 }
    }

    Text {
        anchors.centerIn: parent
        text: root.text
        font.family: "Comic Sans MS"
        font.pixelSize: root.fontSize
        color: root.textColor
        renderType: Text.NativeRendering 
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
    }
}