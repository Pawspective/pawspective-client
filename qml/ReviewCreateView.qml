import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

Rectangle {
    id: root
    color: "#e8d8cb"

    property var viewModel: null
    property string errorMessage: ""

    signal backClicked()
    signal createSuccess()

    QtObject {
        id: theme
        readonly property string fontName: "Comic Sans MS"
        readonly property color purple: "#b8abd7"
        readonly property color fieldBg: "#fdfdfd"
        readonly property color accentPink: "#f4a7b9"
        readonly property color textDark: "#8572af"
        readonly property color buttonText: "#e7ebf5"
        readonly property color errorColor: "#6c63ff"
        readonly property color dropBorder: "#d4c8a8"
    }

    readonly property int maxReviewLength: 2000
    readonly property real contentSpacing: root.height * 0.02
    readonly property real fieldLabelFontSize: root.height * 0.022
    readonly property real fieldValueFontSize: root.height * 0.025
    readonly property real fieldHeight: root.height * 0.06
    readonly property real fieldSpacing: root.height * 0.008
    readonly property real buttonHeight: root.height * 0.08
    readonly property real buttonFontSize: root.height * 0.025
    readonly property real buttonSpacing: root.height * 0.02
    readonly property real loaderSize: root.height * 0.1
    readonly property real bottomPadding: root.height * 0.05

    readonly property int selectedAnimalId: viewModel ? viewModel.animalId : 0

    Connections {
        target: viewModel
        function onErrorOccurred(type, message) {
            root.errorMessage = message
            errorTimer.start()
        }
        function onCreationFinished(success) {
            if (success) {
                root.errorMessage = ""
                root.createSuccess()
            }
        }
    }

    Timer {
        id: errorTimer
        interval: 3000
        onTriggered: root.errorMessage = ""
    }

    Component.onCompleted: {
        if (viewModel) {
            viewModel.initialize()
        }
    }

    Component.onDestruction: {
        if (viewModel) {
            viewModel.cleanup()
        }
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent
        clip: true
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

        ColumnLayout {
            width: scrollView.width
            spacing: root.contentSpacing

            Item { Layout.preferredHeight: root.height * 0.05 }

            Text {
                text: "Create New Review"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                font.family: theme.fontName
                font.pixelSize: root.height * 0.05
                font.bold: true
                color: theme.textDark
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: root.fieldSpacing
                Layout.leftMargin: root.width * 0.05
                Layout.rightMargin: root.width * 0.05

                Text {
                    text: "Animal *"
                    font.family: theme.fontName
                    font.pixelSize: root.fieldLabelFontSize
                    color: theme.textDark
                }

                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.fieldHeight

                    ComboBox {
                        id: animalCombo
                        anchors.fill: parent
                        visible: viewModel && viewModel.availableAnimals && viewModel.availableAnimals.length > 0 && !viewModel.isBusy
                        enabled: viewModel && !viewModel.isBusy
                        model: viewModel ? viewModel.availableAnimals : []
                        textRole: "name"
                        valueRole: "id"
                        currentIndex: {
                            if (!viewModel || !viewModel.availableAnimals || viewModel.availableAnimals.length === 0) {
                                return -1
                            }

                            const index = viewModel.availableAnimals.findIndex(function(item) {
                                return item.id === root.selectedAnimalId
                            })

                            return index >= 0 ? index : 0
                        }
                        onActivated: {
                            if (viewModel) {
                                viewModel.animalId = currentValue
                            }
                        }

                        contentItem: Text {
                            leftPadding: 10
                            text: animalCombo.currentIndex >= 0 ? animalCombo.displayText : "Select animal..."
                            font.family: theme.fontName
                            font.pixelSize: root.fieldValueFontSize
                            verticalAlignment: Text.AlignVCenter
                            color: theme.accentPink
                            elide: Text.ElideRight
                        }

                        background: Rectangle {
                            color: theme.fieldBg
                            radius: 10
                            border.color: theme.dropBorder
                            border.width: 1
                        }

                        indicator: Canvas {
                            id: arrowCanvas
                            x: animalCombo.width - width - 15
                            y: (animalCombo.height - height) / 2
                            width: 12
                            height: 8
                            visible: animalCombo.enabled

                            onPaint: {
                                var ctx = getContext("2d")
                                ctx.reset()
                                ctx.moveTo(0, 0)
                                ctx.lineTo(width, 0)
                                ctx.lineTo(width / 2, height)
                                ctx.closePath()
                                ctx.fillStyle = theme.accentPink
                                ctx.fill()
                            }
                        }

                        popup: Popup {
                            y: animalCombo.height + 3
                            width: animalCombo.width
                            implicitHeight: Math.min(contentItem.implicitHeight, 300)
                            padding: 1
                            background: Rectangle {
                                color: theme.fieldBg
                                border.color: theme.dropBorder
                                radius: 10
                            }
                            contentItem: ListView {
                                clip: true
                                implicitHeight: contentHeight
                                model: animalCombo.popup.visible ? animalCombo.delegateModel : null
                            }
                        }

                        delegate: ItemDelegate {
                            width: animalCombo.width
                            contentItem: Text {
                                text: modelData.name
                                color: hovered ? "white" : theme.textDark
                                font.family: theme.fontName
                                font.pixelSize: root.fieldValueFontSize
                                verticalAlignment: Text.AlignVCenter
                            }
                            background: Rectangle {
                                color: hovered ? theme.purple : "transparent"
                            }
                        }
                    }

                    Rectangle {
                        anchors.fill: parent
                        visible: !viewModel || !viewModel.availableAnimals || viewModel.availableAnimals.length === 0 || viewModel.isBusy
                        color: theme.fieldBg
                        radius: 10
                        border.color: theme.dropBorder
                        border.width: 1

                        Text {
                            anchors.centerIn: parent
                            width: parent.width - 20
                            text: viewModel && viewModel.isBusy ? "Loading animals..." : "There are no animals available for review"
                            font.family: theme.fontName
                            font.pixelSize: root.fieldValueFontSize
                            color: theme.accentPink
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            wrapMode: Text.WordWrap
                        }
                    }
                }

                Text {
                    text: "Review Content *"
                    font.family: theme.fontName
                    font.pixelSize: root.fieldLabelFontSize
                    color: theme.textDark
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.height * 0.3
                    color: theme.fieldBg
                    radius: 10

                    ScrollView {
                        anchors.fill: parent
                        TextArea {
                            width: parent.width
                            height: parent.height
                            id: textInput
                            text: viewModel ? viewModel.text : ""
                            font.family: theme.fontName
                            font.pixelSize: root.fieldValueFontSize
                            color: theme.accentPink
                            wrapMode: TextArea.Wrap
                            leftPadding: 10
                            topPadding: 10
                            enabled: viewModel && !viewModel.isBusy
                            onTextChanged: {
                                if (focus && viewModel) {
                                    viewModel.text = text
                                }
                            }
                            background: null
                            placeholderText: "Write your review here..."
                            placeholderTextColor: theme.accentPink
                        }
                    }
                }

                Text {
                    text: (viewModel ? viewModel.text.length : 0) + " / " + maxReviewLength + " characters"
                    font.family: theme.fontName
                    font.pixelSize: root.fieldLabelFontSize
                    color: (viewModel && viewModel.text.length > maxReviewLength) ? theme.errorColor : theme.textDark
                    Layout.alignment: Qt.AlignRight
                    Layout.fillWidth: true
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: root.buttonSpacing
                Layout.topMargin: root.contentSpacing
                Layout.leftMargin: root.width * 0.05
                Layout.rightMargin: root.width * 0.05

                CustomButton {
                    text: viewModel && viewModel.isBusy
                        ? ((viewModel.availableAnimals && viewModel.availableAnimals.length > 0) ? "Creating..." : "Loading...")
                        : "Create Review"
                    baseColor: theme.purple
                    hoverColor: theme.accentPink
                    textColor: theme.buttonText
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.buttonHeight
                    fontSize: root.buttonFontSize
                    enabled: viewModel && !viewModel.isBusy && viewModel.availableAnimals && viewModel.availableAnimals.length > 0
                    onClicked: {
                        if (viewModel && !viewModel.isBusy) {
                            viewModel.createReview()
                        }
                    }
                }

                CustomButton {
                    text: "Cancel"
                    baseColor: theme.purple
                    hoverColor: theme.accentPink
                    textColor: theme.buttonText
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.buttonHeight
                    fontSize: root.buttonFontSize
                    onClicked: root.backClicked()
                }
            }

            LoaderSpinner {
                Layout.fillWidth: true
                Layout.preferredHeight: root.loaderSize
                Layout.maximumHeight: root.loaderSize
                Layout.minimumHeight: root.loaderSize
                Layout.alignment: Qt.AlignHCenter
                running: viewModel ? viewModel.isBusy : false
                visible: viewModel ? viewModel.isBusy : false
            }

            Label {
                text: root.errorMessage
                color: theme.errorColor
                font.family: theme.fontName
                font.pixelSize: root.fieldLabelFontSize
                visible: text.length > 0
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                Layout.leftMargin: root.width * 0.05
                Layout.rightMargin: root.width * 0.05
            }

            Item { Layout.preferredHeight: root.bottomPadding }
        }
    }
}