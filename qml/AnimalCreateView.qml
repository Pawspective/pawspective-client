import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

Rectangle {
    id: root
    anchors.fill: parent
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

    readonly property real fieldLabelFontSize: root.height * 0.022
    readonly property real fieldValueFontSize: root.height * 0.025
    readonly property real fieldHeight: root.height * 0.06
    readonly property real fieldSpacing: root.height * 0.008
    
    readonly property real contentSpacing: root.height * 0.02
    readonly property real buttonHeight: root.height * 0.08
    readonly property real buttonFontSize: root.height * 0.025
    readonly property real buttonSpacing: root.height * 0.02
    readonly property real loaderSize: root.height * 0.1
    readonly property real bottomPadding: root.height * 0.05

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
                text: "Create New Animal"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                font.family: theme.fontName
                font.pixelSize: root.height * 0.05
                font.bold: true
                color: theme.textDark
            }

            ProfileDataField {
                label: "Animal Name *"
                value: viewModel ? viewModel.name : ""
                onInputFinished: (val) => { if (viewModel) viewModel.name = val }
                Layout.leftMargin: root.width * 0.05
                Layout.rightMargin: root.width * 0.05
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: root.fieldSpacing
                Layout.leftMargin: root.width * 0.05
                Layout.rightMargin: root.width * 0.05
                
                Text {
                    text: "Description (optional)"
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
                            text: viewModel ? viewModel.description : ""
                            font.family: theme.fontName
                            font.pixelSize: root.fieldValueFontSize
                            color: theme.accentPink
                            wrapMode: TextArea.Wrap
                            leftPadding: 10
                            topPadding: 10
                            enabled: viewModel && !viewModel.isBusy
                            onTextChanged: { if (focus && viewModel) viewModel.description = text }
                            background: null
                            placeholderText: "Enter animal description..."
                            placeholderTextColor: theme.accentPink
                        }
                    }
                }
            }

            ComboField {
                label: "Animal Type *"
                model: viewModel ? viewModel.animalTypes : []
                currentValue: viewModel ? viewModel.animalType : ""
                onValueSelected: (val) => { if (viewModel) viewModel.animalType = val }
                Layout.leftMargin: root.width * 0.05
                Layout.rightMargin: root.width * 0.05
            }

            ComboField {
                label: "Breed"
                model: viewModel ? viewModel.breeds : []
                currentValue: viewModel && viewModel.breedId ? String(viewModel.breedId) : ""
                onValueSelected: (val) => { if (viewModel && val) viewModel.breedId = val }
                enabled: viewModel && viewModel.isBreedEnabled && !viewModel.isBusy
                placeholderText: viewModel && viewModel.animalType ? "Select breed..." : "Select animal type first"
                Layout.leftMargin: root.width * 0.05
                Layout.rightMargin: root.width * 0.05
            }

            ComboField {
                label: "Size *"
                model: viewModel ? viewModel.sizes : []
                currentValue: viewModel ? viewModel.size : ""
                onValueSelected: (val) => { if (viewModel) viewModel.size = val }
                Layout.leftMargin: root.width * 0.05
                Layout.rightMargin: root.width * 0.05
            }

            ComboField {
                label: "Gender *"
                model: viewModel ? viewModel.genders : []
                currentValue: viewModel ? viewModel.gender : ""
                onValueSelected: (val) => { if (viewModel) viewModel.gender = val }
                Layout.leftMargin: root.width * 0.05
                Layout.rightMargin: root.width * 0.05
            }

            ProfileDataField {
                label: "Age *"
                value: viewModel && viewModel.age >= 0 ? String(viewModel.age) : ""
                inputMethodHints: Qt.ImhDigitsOnly
                onInputFinished: (val) => {
                    if (viewModel) {
                        var trimmedVal = val.trim()
                        var ageVal = parseInt(trimmedVal, 10)

                        if (trimmedVal === "") {
                            errorMessage = "Age is required."
                            return
                        }

                        if (isNaN(ageVal) || ageVal < 0 || ageVal > 100) {
                            errorMessage = "Age must be a number between 0 and 100."
                            return
                        }

                        errorMessage = ""
                        viewModel.age = ageVal
                    }
                }
                Layout.leftMargin: root.width * 0.05
                Layout.rightMargin: root.width * 0.05
            }

            ComboField {
                label: "Care Level *"
                model: viewModel ? viewModel.careLevels : []
                currentValue: viewModel ? viewModel.careLevel : ""
                onValueSelected: (val) => { if (viewModel) viewModel.careLevel = val }
                Layout.leftMargin: root.width * 0.05
                Layout.rightMargin: root.width * 0.05
            }

            ComboField {
                label: "Color *"
                model: viewModel ? viewModel.colors : []
                currentValue: viewModel ? viewModel.color : ""
                onValueSelected: (val) => { if (viewModel) viewModel.color = val }
                Layout.leftMargin: root.width * 0.05
                Layout.rightMargin: root.width * 0.05
            }

            ComboField {
                label: "Good With *"
                model: viewModel ? viewModel.goodWiths : []
                currentValue: viewModel ? viewModel.goodWith : ""
                onValueSelected: (val) => { if (viewModel) viewModel.goodWith = val }
                Layout.leftMargin: root.width * 0.05
                Layout.rightMargin: root.width * 0.05
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: root.buttonSpacing
                Layout.topMargin: root.contentSpacing
                Layout.leftMargin: root.width * 0.05
                Layout.rightMargin: root.width * 0.05

                CustomButton {
                    text: viewModel && viewModel.isBusy ? "Creating..." : "Create Animal"
                    baseColor: theme.purple
                    hoverColor: theme.accentPink
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.buttonHeight
                    fontSize: root.buttonFontSize
                    isButtonEnabled: viewModel && !viewModel.isBusy
                    onClicked: { if (viewModel) viewModel.createAnimal() }
                }

                CustomButton {
                    text: "Cancel"
                    baseColor: theme.purple
                    hoverColor: theme.accentPink
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.buttonHeight
                    fontSize: root.buttonFontSize
                    isButtonEnabled: viewModel && !viewModel.isBusy
                    onClicked: root.backClicked()
                }
            }

            BusyIndicator {
                Layout.alignment: Qt.AlignHCenter
                implicitWidth: root.loaderSize
                implicitHeight: root.loaderSize
                running: viewModel ? viewModel.isBusy : false
                visible: running
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

   

    component ProfileDataField : ColumnLayout {
        property string label: ""
        property string value: ""
        property alias inputMethodHints: textField.inputMethodHints
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
            id: textField
            text: parent.value
            font.family: theme.fontName
            font.pixelSize: root.fieldValueFontSize
            color: theme.accentPink
            Layout.fillWidth: true
            Layout.preferredHeight: root.fieldHeight
            leftPadding: 10
            background: Rectangle { color: theme.fieldBg; radius: 10 }
            onTextChanged: if(focus) parent.inputFinished(text)
        }
    }

    component ComboField : ColumnLayout {
        property string label: ""
        property var model: []
        property string currentValue: ""
        property string placeholderText: ""
        property bool enabled: true
        signal valueSelected(string value)
        
        Layout.fillWidth: true
        spacing: root.fieldSpacing
        
        Text {
            text: parent.label
            font.family: theme.fontName
            font.pixelSize: root.fieldLabelFontSize
            color: theme.textDark
        }
        
        ComboBox {
            id: comboBox
            Layout.fillWidth: true
            Layout.preferredHeight: root.fieldHeight
            textRole: "text"
            valueRole: "value"
            model: parent.model
            enabled: parent.enabled
            
            currentIndex: {
                if (!parent.currentValue) return 0
                for (var i = 0; i < model.length; i++) {
                    if (model[i] && String(model[i].value) === parent.currentValue) return i
                }
                return 0
            }
            
            onActivated: {
                var val = model[currentIndex] ? model[currentIndex].value : ""
                parent.valueSelected(String(val))
            }
            
            contentItem: Text {
                leftPadding: 10
                text: comboBox.currentIndex > 0 ? comboBox.displayText : (parent.placeholderText || "Select...")
                font.family: theme.fontName
                font.pixelSize: root.fieldValueFontSize
                verticalAlignment: Text.AlignVCenter
                color: theme.accentPink
            }
            
            background: Rectangle { color: theme.fieldBg; radius: 10 }
            
            popup: Popup {
                y: comboBox.height + 3
                width: comboBox.width
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
                    model: comboBox.popup.visible ? comboBox.delegateModel : null
                }
            }

            delegate: ItemDelegate {
                width: comboBox.width
                contentItem: Text {
                    text: modelData.text
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
    }

    component CustomButton : Rectangle {
        id: btnRoot
        property string text: ""
        property color baseColor: "#b8abd7"
        property color hoverColor: "#f4a7b9"
        property real fontSize: 16
        property bool isButtonEnabled: true 
        
        signal clicked()
        
        radius: 10
        color: isButtonEnabled ? (btnMouseArea.containsMouse ? hoverColor : baseColor) : "#ccc"
        
        Text {
            anchors.centerIn: parent
            text: btnRoot.text
            font.family: theme.fontName
            font.pixelSize: btnRoot.fontSize
            color: "white"
        }
        
        MouseArea {
            id: btnMouseArea
            anchors.fill: parent
            hoverEnabled: true
            enabled: btnRoot.isButtonEnabled
            onClicked: btnRoot.clicked()
        }
    }
}