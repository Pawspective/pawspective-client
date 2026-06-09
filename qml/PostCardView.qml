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
        readonly property color postText: "#f4a7b9"
    }

    property int postId: -1
    property string postText: ""
    property var postCreatedAt: null
    property var postPhotos: []
    property bool canEdit: false
    property bool isExpanded: false
    property int previewLimit: 220

    signal editRequested(int postId, string postText, var postCreatedAt, var postPhotos)
    signal deleteRequested(int postId)

    readonly property real padV: root.width * 0.025
    readonly property real padH: root.width * 0.06
    readonly property real titleSize: root.width * 0.022
    readonly property real subtitleSize: root.width * 0.018
    readonly property real descSize: root.width * 0.018
    readonly property bool isExpandable: root.postText.length > root.previewLimit
    readonly property string displayText: buildDisplayText()

    readonly property string createdAtText: formatCreatedAt(root.postCreatedAt)

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
            visible: root.postText.length > 0
            implicitHeight: postTextColumn.implicitHeight + root.padV * 1.2

            ColumnLayout {
                id: postTextColumn
                anchors.fill: parent
                anchors.margins: root.padV * 0.6
                spacing: root.padV * 0.3

                Text {
                    id: postBodyText
                    text: root.displayText
                    textFormat: Text.PlainText
                    font.family: theme.fontName
                    font.pixelSize: root.descSize
                    color: theme.postText
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
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

        Item {
            visible: root.postPhotos && root.postPhotos.length > 0
            Layout.fillWidth: true
            Layout.preferredHeight: root.width * 0.28

            ScrollView {
                anchors.fill: parent
                ScrollBar.vertical.policy: ScrollBar.AlwaysOff
                clip: true

                Row {
                    height: root.width * 0.26
                    spacing: 8

                    Repeater {
                        model: root.postPhotos || []
                        delegate: Rectangle {
                            width: root.width * 0.26
                            height: root.width * 0.26
                            radius: 8
                            clip: true
                            color: theme.pageBg

                            Image {
                                anchors.fill: parent
                                source: storageBaseUrl + modelData
                                fillMode: Image.PreserveAspectCrop
                            }
                        }
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: root.padH * 0.3

            Item {
                Layout.fillWidth: true
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
                    root.editRequested(root.postId, root.postText, root.postCreatedAt, root.postPhotos)
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
                    root.deleteRequested(root.postId)
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
        var rawText = root.postText || ""
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