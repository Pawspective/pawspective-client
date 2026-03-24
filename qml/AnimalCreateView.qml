import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

Rectangle {
    id: root
    color: "#e8d8cb"

    property string errorMessage: ""

    signal backClicked()
    signal createSuccess()

    property var animalTypes: [
        { text: "Dog", value: "Dog" },
        { text: "Cat", value: "Cat" },
        { text: "Other", value: "Other" }
    ]
    
    property var breeds: [
        { text: "Labrador", value: 1 },
        { text: "German Shepherd", value: 2 },
        { text: "Bulldog", value: 3 },
        { text: "Persian Cat", value: 4 },
        { text: "Siamese Cat", value: 5 },
        { text: "Mixed", value: 6 }
    ]
    
    property var sizes: [
        { text: "Small", value: "Small" },
        { text: "Medium", value: "Medium" },
        { text: "Large", value: "Large" }
    ]
    
    property var genders: [
        { text: "Male", value: "Male" },
        { text: "Female", value: "Female" },
        { text: "Unknown", value: "Unknown" }
    ]
    
    property var careLevels: [
        { text: "Easy", value: "Easy" },
        { text: "Moderate", value: "Moderate" },
        { text: "Difficult", value: "Difficult" },
        { text: "Special Needs", value: "SpecialNeeds" }
    ]
    
    property var colors: [
        { text: "Black", value: "Black" },
        { text: "White", value: "White" },
        { text: "Brown", value: "Brown" },
        { text: "Grey", value: "Grey" },
        { text: "Orange", value: "Orange" },
        { text: "Cream", value: "Cream" },
        { text: "Golden", value: "Golden" },
        { text: "Spotted", value: "Spotted" },
        { text: "Striped", value: "Striped" },
        { text: "Mixed", value: "Mixed" }
    ]
    
    property var goodWiths: [
        { text: "Dogs", value: "Dogs" },
        { text: "Cats", value: "Cats" },
        { text: "Children", value: "Children" },
        { text: "Elderly", value: "Elderly" }
    ]

    property string animalName: ""
    property string description: ""
    property int selectedAnimalTypeIndex: 0
    property int selectedBreedIndex: 0
    property int selectedSizeIndex: 0
    property int selectedGenderIndex: 0
    property string age: ""
    property int selectedCareLevelIndex: 0
    property int selectedColorIndex: 0
    property int selectedGoodWithIndex: 0
    
    property bool isBusy: false

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

    function createAnimal() {
        if (!animalName.trim()) {
            errorMessage = "Please enter animal name"
            return
        }
        
        errorMessage = ""
        isBusy = true
        createTimer.start()
    }
    
    Timer {
        id: createTimer
        interval: 2000
        onTriggered: {
            isBusy = false
            createSuccess()
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
                text: "Create New Animal"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                font.family: theme.fontName
                font.pixelSize: root.height * 0.05
                font.bold: true
                color: theme.textDark
                Layout.bottomMargin: 10
            }

            // Name field
            ProfileDataField {
                label: "Animal Name *"
                value: root.animalName
                onInputFinished: (val) => root.animalName = val
            }

            // Description field
            ColumnLayout {
                Layout.fillWidth: true
                spacing: root.fieldSpacing
                
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
                            text: root.description
                            font.family: theme.fontName
                            font.pixelSize: root.fieldValueFontSize
                            color: theme.accentPink
                            wrapMode: TextArea.Wrap
                            leftPadding: root.fieldLeftMargin
                            topPadding: 10
                            enabled: !root.isBusy
                            onTextChanged: {
                                if (focus) root.description = text
                            }
                            background: null
                            placeholderText: "Enter animal description..."
                            placeholderTextColor: theme.accentPink
                        }
                    }
                }
            }

            // Animal Type combo
            ComboField {
                label: "Animal Type"
                model: root.animalTypes
                currentIndex: root.selectedAnimalTypeIndex
                onSelectedIndexChanged: (index) => root.selectedAnimalTypeIndex = index
            }

            // Breed combo
            ComboField {
                label: "Breed"
                model: root.breeds
                currentIndex: root.selectedBreedIndex
                onSelectedIndexChanged: (index) => root.selectedBreedIndex = index
            }

            // Size combo
            ComboField {
                label: "Size"
                model: root.sizes
                currentIndex: root.selectedSizeIndex
                onSelectedIndexChanged: (index) => root.selectedSizeIndex = index
            }

            // Gender combo
            ComboField {
                label: "Gender"
                model: root.genders
                currentIndex: root.selectedGenderIndex
                onSelectedIndexChanged: (index) => root.selectedGenderIndex = index
            }

            // Age field
            ProfileDataField {
                label: "Age (in months)"
                value: root.age
                onInputFinished: (val) => root.age = val
            }

            // Care Level combo
            ComboField {
                label: "Care Level"
                model: root.careLevels
                currentIndex: root.selectedCareLevelIndex
                onSelectedIndexChanged: (index) => root.selectedCareLevelIndex = index
            }

            // Color combo
            ComboField {
                label: "Color"
                model: root.colors
                currentIndex: root.selectedColorIndex
                onSelectedIndexChanged: (index) => root.selectedColorIndex = index
            }

            // Good With combo
            ComboField {
                label: "Good With"
                model: root.goodWiths
                currentIndex: root.selectedGoodWithIndex
                onSelectedIndexChanged: (index) => root.selectedGoodWithIndex = index
            }

            // Buttons
            ColumnLayout {
                Layout.fillWidth: true
                spacing: root.buttonSpacing
                Layout.topMargin: root.contentSpacing

                CustomButton {
                    text: root.isBusy ? "Creating..." : "Create Animal"
                    baseColor: theme.purple
                    hoverColor: theme.accentPink
                    textColor: theme.buttonText
                    fontSize: root.buttonFontSize
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.buttonHeight
                    enabled: !root.isBusy
                    onClicked: root.createAnimal()
                }

                CustomButton {
                    text: "Cancel"
                    baseColor: theme.purple
                    hoverColor: theme.accentPink
                    textColor: theme.buttonText
                    fontSize: root.buttonFontSize
                    Layout.fillWidth: true
                    Layout.preferredHeight: root.buttonHeight
                    enabled: !root.isBusy
                    onClicked: root.backClicked()
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
                running: root.isBusy
                visible: root.isBusy
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
                Layout.bottomMargin: root.height * 0.05
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
            enabled: !root.isBusy
            background: Rectangle { color: theme.fieldBg; radius: 10 }
            onTextChanged: if(focus) parent.inputFinished(text)
        }
    }

    component ComboField : ColumnLayout {
        property string label: ""
        property var model: []
        property int currentIndex: -1
        signal selectedIndexChanged(int index)
        
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
            model: parent.model
            currentIndex: parent.currentIndex
            
            onActivated: (index) => parent.selectedIndexChanged(index)
            
            indicator: Canvas {
                x: comboBox.width - width - 15
                y: (comboBox.height - height) / 2
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
                text: comboBox.displayText
                font.family: theme.fontName
                font.pixelSize: root.fieldValueFontSize
                verticalAlignment: Text.AlignVCenter
                color: theme.accentPink
                elide: Text.ElideRight
            }
            
            background: Rectangle { 
                color: theme.fieldBg
                radius: 10 
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
                    elide: Text.ElideRight
                }
                background: Rectangle {
                    color: (parent.hovered || parent.highlighted) ? theme.purple : "transparent"
                }
            }
            
            popup: Popup {
                y: comboBox.height + 3
                width: comboBox.width
                implicitHeight: contentItem.implicitHeight
                padding: 1
                
                contentItem: ListView {
                    clip: true
                    implicitHeight: contentHeight
                    model: comboBox.delegateModel
                    currentIndex: comboBox.highlightedIndex
                    ScrollIndicator.vertical: ScrollIndicator {}
                }
                
                background: Rectangle {
                    color: theme.fieldBg
                    border.color: theme.dropBorder
                    radius: 10
                }
            }
        }
    }
}