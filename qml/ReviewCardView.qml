import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    QtObject {
        id: theme
        readonly property string fontName: "Comic Sans MS"
        readonly property color pageBg: "#e8d8cb"
        readonly property color purple: "#b8abd7"
        readonly property color fieldBg: "#fdfdfd"
        readonly property color accentPink: "#f4a7b9"
        readonly property color textDark: "#8572af"
        readonly property color buttonText: "#e7ebf5"
        readonly property color reviewText: '#f4a7b9'
    }

    property int reviewId: -1
    property int animalId: -1
    property string animalName: ""
    property string reviewText: ""
    property var reviewCreatedAt: null
    property bool canEdit: false
    property bool isExpanded: false
    property int previewLimit: 220

    signal animalClicked(int animalId)
    signal editRequested(int reviewId)
    signal deleteRequested(int reviewId)

    readonly property real padV: root.width * 0.025
    readonly property real padH: root.width * 0.06
    readonly property real titleSize: root.width * 0.022
    readonly property real subtitleSize: root.width * 0.018
    readonly property real descSize: root.width * 0.018
    readonly property bool isExpandable: root.reviewText.length > root.previewLimit
    readonly property string displayText: buildDisplayText()

    readonly property string createdAtText: formatCreatedAt(root.reviewCreatedAt)

    radius: root.width * 0.015
    color: theme.purple

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
        z: 1

        Rectangle {
            Layout.fillWidth: true
            radius: root.width * 0.012
            color: theme.fieldBg
            border.color: theme.fieldBg
            border.width: 1
            visible: root.reviewText.length > 0
            implicitHeight: reviewTextColumn.implicitHeight + root.padV * 1.2

            ColumnLayout {
                id: reviewTextColumn
                anchors.fill: parent
                anchors.margins: root.padV * 0.6
                spacing: root.padV * 0.3

                Text {
                    id: reviewBodyText
                    text: root.displayText
                    textFormat: Text.PlainText
                    font.family: theme.fontName
                    font.pixelSize: root.descSize
                    color: theme.reviewText
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }

                Item {
                    Layout.fillWidth: true
                    implicitHeight: toggleText.implicitHeight
                    visible: root.isExpandable

                    Text {
                        id: toggleText
                        text: root.isExpanded ? "Show less" : "Show more"
                        font.family: theme.fontName
                        font.pixelSize: root.descSize
                        color: theme.textDark
                        font.underline: toggleArea.containsMouse
                    }

                    MouseArea {
                        id: toggleArea
                        anchors.fill: toggleText
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: root.isExpanded = !root.isExpanded
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: root.padH * 0.3

            RowLayout {
                Layout.fillWidth: true
                spacing: root.padH * 0.15

                Text {
                    text: "Adopted pet name:"
                    font.family: theme.fontName
                    font.pixelSize: root.subtitleSize
                    color: theme.fieldBg
                }

                Item {
                    Layout.fillWidth: true
                    implicitHeight: adoptedNameText.implicitHeight

                    Text {
                        id: adoptedNameText
                        text: root.animalName.length > 0 ? root.animalName : "-"
                        font.family: theme.fontName
                        font.pixelSize: root.subtitleSize
                        font.underline: adoptedNameArea.containsMouse
                        color: theme.fieldBg
                        elide: Text.ElideRight
                        width: parent.width
                    }

                    MouseArea {
                        id: adoptedNameArea
                        anchors.fill: adoptedNameText
                        hoverEnabled: true
                        enabled: root.animalId > 0 && root.animalName.length > 0
                        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                        onClicked: function(mouse) {
                            mouse.accepted = true
                            if (root.animalId > 0) {
                                root.animalClicked(root.animalId)
                            }
                        }
                    }
                }
            }

            Text {
                visible: root.createdAtText.length > 0
                text: root.createdAtText
                font.family: theme.fontName
                font.pixelSize: root.subtitleSize
                color: theme.fieldBg
                elide: Text.ElideRight
                Layout.alignment: Qt.AlignRight | Qt.AlignTop
            }
        }
    }

    Rectangle {
        id: optionsButton
        width: root.width * 0.034
        height: width
        radius: width / 2
        color: optionsArea.containsMouse ? "#19000000" : "transparent"
        border.color: "transparent"
        border.width: 0
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: root.padV * 0.4
        anchors.rightMargin: root.padH * 0.25
        visible: root.canEdit
        z: 2

        Row {
            anchors.centerIn: parent
            spacing: Math.max(1, Math.round(parent.width * 0.08))

            Repeater {
                model: 3
                delegate: Rectangle {
                    width: Math.max(2, Math.round(parent.width * 0.22))
                    height: width
                    radius: width / 2
                    color: theme.fieldBg
                }
            }
        }

        MouseArea {
            id: optionsArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: optionsPopup.open()
        }
    }

    Popup {
        id: optionsPopup
        parent: root
        modal: false
        focus: true
        padding: 6
        closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape
        x: optionsButton.x + optionsButton.width - width
        y: optionsButton.y + optionsButton.height + root.padV * 0.3

        background: Rectangle {
            radius: 8
            color: theme.fieldBg
            border.color: theme.purple
            border.width: 1
        }

        contentItem: Column {
            spacing: 4

            ItemDelegate {
                width: Math.max(120, implicitWidth)
                text: "Edit"
                contentItem: Text {
                    text: parent.text
                    font.family: theme.fontName
                    font.pixelSize: root.subtitleSize
                    color: theme.textDark
                }
                background: Rectangle {
                    color: parent.hovered ? theme.accentPink : "transparent"
                    radius: 6
                }
                onClicked: {
                    optionsPopup.close()
                    root.editRequested(root.reviewId)
                }
            }

            ItemDelegate {
                width: Math.max(120, implicitWidth)
                text: "Delete"
                contentItem: Text {
                    text: parent.text
                    font.family: theme.fontName
                    font.pixelSize: root.subtitleSize
                    color: theme.textDark
                }
                background: Rectangle {
                    color: parent.hovered ? theme.accentPink : "transparent"
                    radius: 6
                }
                onClicked: {
                    optionsPopup.close()
                    root.deleteRequested(root.reviewId)
                }
            }
        }
    }

    function formatCreatedAt(value) {
        if (!value) {
            return ""
        }
        if (value instanceof Date) {
            return Qt.formatDateTime(value, "yyyy-MM-dd")
        }
        if (typeof value === "string") {
            return value
        }
        if (value.toString) {
            return value.toString()
        }
        return ""
    }

    function buildDisplayText() {
        var rawText = root.reviewText || ""
        if (!root.isExpandable) {
            return rawText
        }

        if (root.isExpanded) {
            return rawText
        }

        var preview = rawText.slice(0, root.previewLimit).trim()
        if (preview.length === 0) {
            return rawText
        }
        return preview + "..."
    }
}
