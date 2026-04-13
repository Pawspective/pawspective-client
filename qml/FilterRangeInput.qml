import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: root

    property string labelText: "Age Range"
    property var valueMin: null
    property var valueMax: null
    
    QtObject {
        id: theme
        readonly property string fontName: "Comic Sans MS"
        readonly property color pageBg: "#e8d8cb"
        readonly property color sidebarBg: "#e9bebb"
        readonly property color purple: "#b8abd7"
        readonly property color fieldBg: "#fdfdfd"
        readonly property color accentPink: "#f4a7b9"
        readonly property color textDark: "#8572af"
        readonly property color buttonText: "#e7ebf5"
    }

    implicitWidth: 300
    implicitHeight: contentColumn.implicitHeight + topPadding + bottomPadding
    padding: 8

    background: Rectangle {
        radius: 14
        color: theme.pageBg
        border.color: theme.purple
        border.width: 1
    }

    contentItem: ColumnLayout {
        id: contentColumn
        spacing: 6

        Text {
            text: root.labelText
            font.family: theme.fontName
            font.pixelSize: 14
            font.bold: true
            color: theme.textDark
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                Text {
                    text: "Min"
                    font.family: theme.fontName
                    font.pixelSize: 11
                    color: theme.textDark
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 40
                    radius: 8
                    color: theme.fieldBg
                    border.color: theme.purple
                    border.width: 1

                    TextField {
                        id: minField
                        anchors.fill: parent
                        anchors.margins: 2
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        placeholderText: "Input min age"
                        placeholderTextColor: theme.accentPink
                        color: theme.textDark
                        font.family: theme.fontName
                        selectByMouse: true
                        validator: IntValidator { bottom: 0; top: 999 }
                        background: Rectangle { color: "transparent" }

                        onTextChanged: {
                            if (text.trim() === "") {
                                root.valueMin = null
                            } else {
                                var val = parseInt(text) || 0
                                root.valueMin = Math.max(0, val)
                            }
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4

                Text {
                    text: "Max"
                    font.family: theme.fontName
                    font.pixelSize: 11
                    color: theme.textDark
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 40
                    radius: 8
                    color: theme.fieldBg
                    border.color: theme.purple
                    border.width: 1

                    TextField {
                        id: maxField
                        anchors.fill: parent
                        anchors.margins: 2
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        placeholderText: "Input max age"
                        placeholderTextColor: theme.accentPink
                        color: theme.textDark
                        font.family: theme.fontName
                        selectByMouse: true
                        validator: IntValidator { bottom: 0; top: 999 }
                        background: Rectangle { color: "transparent" }

                        onTextChanged: {
                            if (text.trim() === "") {
                                root.valueMax = null
                            } else {
                                var val = parseInt(text) || 0
                                root.valueMax = Math.max(0, val)
                            }
                        }
                    }
                }
            }
        }
    }
}
