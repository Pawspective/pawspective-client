import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

Rectangle {
    id: root
    color: "#e8d8cb"

    property var viewModel: updateOrganizationViewModel
    property string errorMessage: ""

    signal saveCompleted()
    signal discard()

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

    readonly property real fieldLabelFontSize: root.height * 0.022
    readonly property real fieldValueFontSize: root.height * 0.025
    readonly property real fieldHeight: root.height * 0.06
    readonly property real fieldSpacing: root.height * 0.008
    readonly property real fieldLeftMargin: root.width * 0.01
    
    readonly property real contentSpacing: root.height * 0.02
    readonly property real buttonHeight: root.height * 0.08
    readonly property real buttonFontSize: root.height * 0.025
    readonly property real buttonSpacing: root.height * 0.02
    readonly property real loaderSize: root.height * 0.1
    readonly property real loaderTopMargin: 10

    Connections {
        target: viewModel
        function onSaveFailed(message) {
            root.errorMessage = message
        }
        function onSaveCompleted() {
            root.errorMessage = ""
            stackView.pop()
        }
    }

    Component.onCompleted: {
        if (viewModel) {
            viewModel.initialize();
        }
    }

    ScrollView {
        anchors.fill: parent
        contentWidth: parent.width
        clip: true

        ColumnLayout {
            width: parent.width * 0.9
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: root.contentSpacing
            anchors.top: parent.top
            anchors.topMargin: root.height * 0.05

            Text {
                text: "Update Organization"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                font.family: theme.fontName
                font.pixelSize: root.height * 0.05
                font.bold: true
                color: theme.textDark
                Layout.bottomMargin: 10
            }

            ProfileDataField {
                label: "Organization Name"
                value: viewModel.name
                onInputFinished: (val) => viewModel.name = val
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: root.fieldSpacing
                
                Text {
                    text: "City"
                    font.family: theme.fontName
                    font.pixelSize: root.fieldLabelFontSize
                    color: theme.textDark
                }

                ComboBox {
                    id: cityCombo
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.fieldHeight
                    textRole: "text"
                    model: viewModel.cities
                    currentIndex: viewModel.currentCityIndex

                    onActivated: (index) => viewModel.cityId = model[index].value

                    indicator: Canvas {
                        x: cityCombo.width - width - 15
                        y: (cityCombo.height - height) / 2
                        width: 12
                        height: 8
                        onPaint: {
                            var context = getContext("2d");
                            context.reset();
                            context.moveTo(0, 0);
                            context.lineTo(width, 0);
                            context.lineTo(width / 2, height);
                            context.closePath();
                            context.fillStyle = theme.accentPink;
                            context.fill();
                        }
                    }

                    contentItem: Text {
                        leftPadding: root.fieldLeftMargin
                        text: cityCombo.displayText
                        font.family: theme.fontName
                        font.pixelSize: root.fieldValueFontSize
                        verticalAlignment: Text.AlignVCenter
                        color: theme.accentPink
                    }

                    background: Rectangle { color: theme.fieldBg; radius: 10 }

                    delegate: ItemDelegate {
                        width: cityCombo.width
                        hoverEnabled: true
                        contentItem: Text {
                            text: modelData.text
                            color: (parent.hovered || parent.highlighted) ? "white" : theme.textDark
                            font.family: theme.fontName
                            font.pixelSize: root.fieldValueFontSize
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            color: (parent.hovered || parent.highlighted) ? theme.purple : "transparent"
                        }
                    }

                    popup: Popup {
                        y: cityCombo.height + 3
                        width: cityCombo.width
                        implicitHeight: contentItem.implicitHeight
                        padding: 1
                        background: Rectangle {
                            color: theme.fieldBg
                            border.color: theme.dropBorder
                            radius: 10
                        }
                        contentItem: ListView {
                            clip: true
                            implicitHeight: contentHeight
                            model: cityCombo.popup.visible ? cityCombo.delegateModel : null
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: root.fieldSpacing
                
                Text {
                    text: "Description"
                    font.family: theme.fontName
                    font.pixelSize: root.fieldLabelFontSize
                    color: theme.textDark
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.height * 0.2
                    color: theme.fieldBg
                    radius: 10

                    ScrollView {
                        anchors.fill: parent
                        TextArea {
                            id: descInput
                            text: viewModel.description
                            font.family: theme.fontName
                            font.pixelSize: root.fieldValueFontSize
                            color: theme.accentPink
                            wrapMode: TextArea.Wrap
                            leftPadding: root.fieldLeftMargin
                            topPadding: 10
                            enabled: !viewModel.isBusy
                            onTextChanged: if(focus) viewModel.description = text
                            background: null
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: root.buttonSpacing
                Layout.topMargin: root.contentSpacing

                CustomButton {
                    text: viewModel.isBusy ? "Saving..." : "Save Changes"
                    baseColor: viewModel.isDirty ? theme.purple : "#cccccc"
                    hoverColor: viewModel.isDirty ? theme.accentPink : "#cccccc"
                    textColor: theme.buttonText
                    fontSize: root.buttonFontSize
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.buttonHeight
                    enabled: !viewModel.isBusy && viewModel.isDirty
                    onClicked: viewModel.saveChanges()
                }

                CustomButton {
                    text: "Discard Changes"
                    baseColor: theme.purple
                    hoverColor: theme.accentPink
                    textColor: theme.buttonText
                    fontSize: root.buttonFontSize
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.buttonHeight
                    enabled: !viewModel.isBusy
                    onClicked: {
                        viewModel.discardChanges();
                        root.discard();
                    }
                }
            }

            LoaderSpinner {
                Layout.fillWidth: true
                Layout.preferredHeight: root.loaderSize
                Layout.maximumHeight: root.loaderSize
                Layout.minimumHeight: root.loaderSize
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: root.loaderTopMargin
                Layout.bottomMargin: root.contentSpacing
                running: viewModel.isBusy
                visible: viewModel.isBusy
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
            }

            Item { Layout.fillHeight: true }
        }
    }

    component ProfileDataField : ColumnLayout {
        property string label: ""
        property string value: ""
        signal inputFinished(string newValue)
        Layout.fillWidth: true
        spacing: root.fieldSpacing
        Text {
            text: parent.label
            font.family: theme.fontName
            font.pixelSize: root.fieldLabelFontSize
            color: theme.textDark
        }
        TextField {
            text: parent.value
            font.family: theme.fontName
            font.pixelSize: root.fieldValueFontSize
            color: theme.accentPink
            Layout.fillWidth: true
            Layout.preferredHeight: root.fieldHeight
            leftPadding: root.fieldLeftMargin
            enabled: !viewModel.isBusy
            background: Rectangle { color: theme.fieldBg; radius: 10 }
            onTextChanged: if (focus) inputFinished(text)
        }
    }
}