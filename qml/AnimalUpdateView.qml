import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

Rectangle {
    id: root
    color: "#e8d8cb"

    property var viewModel: null
    property string errorMessage: ""

    signal discard()
    signal saveCompleted()

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
    readonly property real bottomPadding: root.height * 0.05

    Connections {
        target: viewModel
        function onLoadFailed(message) {
            root.errorMessage = message
            errorTimer.start()
        }
        function onSaveFailed(message) {
            root.errorMessage = message
            errorTimer.start()
        }
        function onSaveCompleted() {
            root.errorMessage = ""
            root.saveCompleted()
        }
        function onLoadCompleted() {
            root.errorMessage = ""
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
                text: "Update Animal"
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
                label: "Breed *"
                model: viewModel ? viewModel.breeds : []
                currentValue: viewModel && viewModel.breedId ? String(viewModel.breedId) : ""
                onValueSelected: (val) => { 
                    if (viewModel && val) {
                        viewModel.breedId = parseInt(val)
                    }
                }
                enabled: viewModel && viewModel.isBreedEnabled && !viewModel.isBusy
                placeholderText: viewModel && viewModel.animalType ? "Select breed..." : "Select animal type first"
                required: true
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
                            errorTimer.start()
                            return
                        }

                        if (isNaN(ageVal) || ageVal < 0 || ageVal > 100) {
                            errorMessage = "Age must be a number between 0 and 100."
                            errorTimer.start()
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

            ComboField {
                label: "Status *"
                model: viewModel ? viewModel.statuses : []
                currentValue: viewModel ? viewModel.status : ""
                onValueSelected: (val) => { if (viewModel) viewModel.status = val }
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
                    text: viewModel && viewModel.isBusy ? "Saving..." : "Save Changes"
                    baseColor: (viewModel && viewModel.isDirty) ? theme.purple : "#cccccc"
                    hoverColor: (viewModel && viewModel.isDirty) ? theme.accentPink : "#cccccc"
                    textColor: theme.buttonText
                    fontSize: root.buttonFontSize
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.buttonHeight
                    enabled: viewModel && !viewModel.isBusy && viewModel.isDirty
                    onClicked: { 
                        if (viewModel) {
                            if (!viewModel.breedId || viewModel.breedId <= 0) {
                                errorMessage = "Please select a breed."
                                errorTimer.start()
                                return
                            }
                            viewModel.saveChanges()
                        }
                    }
                }

                CustomButton {
                    text: "Discard Changes"
                    baseColor: theme.purple
                    hoverColor: theme.accentPink
                    textColor: theme.buttonText
                    fontSize: root.buttonFontSize
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.buttonHeight
                    enabled: viewModel && !viewModel.isBusy
                    onClicked: {
                        if (viewModel) viewModel.discardChanges()
                        root.discard()
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
                running: viewModel ? viewModel.isBusy : false
                visible: viewModel ? viewModel.isBusy : false
            }

            Item { Layout.preferredHeight: root.bottomPadding }
        }
    }

    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 20
        height: errorText.implicitHeight + 20
        visible: root.errorMessage.length > 0
        color: theme.errorColor
        radius: 10
        z: 100

        Text {
            id: errorText
            text: root.errorMessage
            color: "white"
            font.family: theme.fontName
            font.pixelSize: root.fieldLabelFontSize
            wrapMode: Text.WordWrap
            anchors.centerIn: parent
            width: parent.width - 20
            horizontalAlignment: Text.AlignHCenter
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
            enabled: viewModel && !viewModel.isBusy
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
        property bool required: false
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
            enabled: parent.enabled && viewModel && !viewModel.isBusy
            
            currentIndex: {
                if (!parent.currentValue || parent.currentValue === "") return -1
                for (var i = 0; i < model.length; i++) {
                    if (model[i] && String(model[i].value) === parent.currentValue) return i
                }
                return -1
            }
            
            onActivated: {
                var val = model[currentIndex] ? model[currentIndex].value : ""
                parent.valueSelected(String(val))
            }
            
            contentItem: Text {
                leftPadding: 10
                text: comboBox.currentIndex >= 0 ? comboBox.displayText : (parent.placeholderText || "Select...")
                font.family: theme.fontName
                font.pixelSize: root.fieldValueFontSize
                verticalAlignment: Text.AlignVCenter
                color: theme.accentPink
                elide: Text.ElideRight
            }
            
            background: Rectangle { 
                color: theme.fieldBg
                radius: 10
                border.color: parent.required && (!parent.currentValue || parent.currentValue === "") ? theme.errorColor : "transparent"
                border.width: parent.required && (!parent.currentValue || parent.currentValue === "") ? 2 : 0
            }
            
            indicator: Canvas {
                id: arrowCanvas
                x: comboBox.width - width - 15
                y: (comboBox.height - height) / 2
                width: 12
                height: 8
                visible: comboBox.enabled
                
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
        }
        
        Text {
            visible: parent.required && (!parent.currentValue || parent.currentValue === "")
            text: "*"
            color: theme.errorColor
            font.pixelSize: root.fieldValueFontSize
            font.bold: true
            Layout.leftMargin: 5
        }
    }

    component CustomButton : Rectangle {
        id: btnRoot
        property string text: ""
        property color baseColor: "#b8abd7"
        property color hoverColor: "#f4a7b9"
        property color textColor: "#e7ebf5"
        property real fontSize: 16
        property bool enabled: true
        
        signal clicked()
        
        radius: 10
        color: enabled ? (btnMouseArea.containsMouse ? hoverColor : baseColor) : "#cccccc"
        
        Text {
            anchors.centerIn: parent
            text: btnRoot.text
            font.family: theme.fontName
            font.pixelSize: btnRoot.fontSize
            color: btnRoot.textColor
        }
        
        MouseArea {
            id: btnMouseArea
            anchors.fill: parent
            hoverEnabled: true
            enabled: btnRoot.enabled
            onClicked: btnRoot.clicked()
        }
    }
}

//TODO: отображение загрузки в create animal и update animal
//TODO: нельзя сохранять, когда пустое поле для породы 