import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    //anchors.fill: parent
    color: "#e8d8cb"

    QtObject {
        id: theme
        readonly property string fontName: "Comic Sans MS"
        readonly property color bgMain: "#b2bb7d"
        readonly property color bgInput: "#fdfdfd"
        readonly property color accentPink: "#f4a7b9"
        readonly property color accentYellow: "#ede8b0"
        readonly property color textMain: "#f1e9bb"
        readonly property color textError: "#6c63ff"
        readonly property color textButton: "#b2bb7d"
        readonly property color dropBorder: "#d4c8a8"
    }

    property string photoUrl: ""
        property string errorMessage: ""

    signal backClicked()
    signal registerSuccess()

    Component.onCompleted: registerOrganizationViewModel.initialize()

    Connections {
        target: registerOrganizationViewModel
        function onRegistrationFinished(success) {
            if (success) {
                root.registerSuccess()
            }
        }
        function onErrorOccurred(type, message) {
                root.errorMessage = message
        }
    }

    Rectangle {
        id: card
        width: 520
        height: contentColumn.implicitHeight + 80
        radius: 20
        color: theme.bgMain
        anchors.centerIn: parent

        ColumnLayout {
            id: contentColumn
            anchors {
                top: parent.top
                topMargin: 45
                left: parent.left
                right: parent.right
                leftMargin: parent.width * 0.1
                rightMargin: parent.width * 0.1
                bottom: parent.bottom
                bottomMargin: 20
            }
            spacing: 12

            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 80
                
                Label {
                    text: "Create"
                    font.family: theme.fontName
                    font.pixelSize: 32
                    color: theme.textMain
                    anchors.centerIn: parent
                }

                Image {
                    id: catIcon
                    source: "../resources/heart_dog.png"
                    fillMode: Image.PreserveAspectFit
                    width: 120; height: 120
                    anchors {
                        left: parent.left
                        bottom: parent.bottom
                        bottomMargin: -30
                    }
                    z: 2 
                }
            }

            TextField {
                id: nameField
                placeholderText: "Name"
                text: registerOrganizationViewModel.name
                onTextChanged: registerOrganizationViewModel.name = text
                placeholderTextColor: theme.accentPink
                color: theme.accentPink
                font.family: theme.fontName
                Layout.fillWidth: true
                leftPadding: 15
                enabled: !registerOrganizationViewModel.isBusy
                background: Rectangle { color: theme.bgInput; radius: 8 }
            }

            Label {
                text: "Description (optional)"
                font.family: theme.fontName
                font.pixelSize: 13
                color: theme.textMain
                Layout.bottomMargin: -6
            }


            TextArea {
                id: descriptionField
                placeholderText: "..."
                text: registerOrganizationViewModel.description
                onTextChanged: registerOrganizationViewModel.description = text
                color: theme.accentPink
                font.family: theme.fontName
                wrapMode: TextArea.Wrap
                Layout.fillWidth: true
                Layout.preferredHeight: 80
                leftPadding: 15
                topPadding: 10
                enabled: !registerOrganizationViewModel.isBusy
                background: Rectangle { color: theme.bgInput; radius: 8 }

                Text {
                    text: descriptionField.placeholderText
                    color: theme.accentPink
                    font: descriptionField.font
                    leftPadding: descriptionField.leftPadding
                    topPadding: descriptionField.topPadding
                    visible: !descriptionField.text && !descriptionField.activeFocus
                    opacity: 0.7
                }
            }

            Label {
                text: "City"
                font.family: theme.fontName
                font.pixelSize: 13
                color: theme.textMain
                Layout.bottomMargin: -6
            }

            ComboBox {
                id: cityCombo
                Layout.fillWidth: true
                Layout.preferredHeight: 42
                enabled: !registerOrganizationViewModel.isBusy
                model: registerOrganizationViewModel.cities
                textRole: "text"
                valueRole: "value"
                displayText: currentIndex < 0 ? "Select city..." : currentText

                onCurrentValueChanged: {
                    const value = Number(cityCombo.currentValue)
                    registerOrganizationViewModel.cityId = Number.isFinite(value) ? value : 0
                }

                background: Rectangle {
                    color: theme.bgInput
                    radius: 8
                }

                contentItem: Text {
                    leftPadding: 15
                    text: cityCombo.displayText
                    font.family: theme.fontName
                    font.pixelSize: 14
                    color: theme.accentPink
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                indicator: Text {
                    text: "▾"
                    font.pixelSize: 14
                    color: theme.accentPink
                    anchors {
                        right: parent.right
                        rightMargin: 12
                        verticalCenter: parent.verticalCenter
                    }
                }

                popup: Popup {
                    y: cityCombo.height + 2
                    width: cityCombo.width
                    implicitHeight: contentItem.implicitHeight
                    padding: 1

                    contentItem: ListView {
                        clip: true
                        implicitHeight: contentHeight
                        model: cityCombo.popup.visible ? cityCombo.delegateModel : null

                        ScrollIndicator.vertical: ScrollIndicator {}
                    }

                    background: Rectangle {
                        color: theme.bgInput
                        radius: 8
                        border.color: theme.dropBorder
                        border.width: 1
                    }
                }

                delegate: ItemDelegate {
                    width: cityCombo.width
                    contentItem: Text {
                        text: modelData.text
                        font.family: theme.fontName
                        font.pixelSize: 14
                        color: theme.accentPink
                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignVCenter
                    }
                    highlighted: cityCombo.highlightedIndex === index
                    background: Rectangle {
                        color: highlighted ? theme.accentYellow : theme.bgInput
                    }
                }
            }

            CustomButton {
                id: submitBtn
                text: registerOrganizationViewModel.isBusy ? "Creating..." : "Create organization"
                baseColor: theme.accentYellow
                hoverColor: theme.accentPink
                textColor: theme.textButton
                Layout.fillWidth: true
                Layout.topMargin: 4
                enabled: !registerOrganizationViewModel.isBusy
                onClicked: {
                        root.errorMessage = ""
                    registerOrganizationViewModel.registerOrganization()
                }
            }

            CustomButton {
                id: cancelBtn
                text: "Cancel"
                baseColor: theme.accentYellow
                hoverColor: theme.accentPink
                textColor: theme.textButton
                Layout.fillWidth: true
                enabled: !registerOrganizationViewModel.isBusy
                onClicked: root.backClicked()
            }

            LoaderSpinner {
                running: registerOrganizationViewModel.isBusy
                visible: registerOrganizationViewModel.isBusy
                Layout.alignment: Qt.AlignHCenter
            }

            Label {
                id: errorMessageLabel
                    text: root.errorMessage
                color: theme.textError
                font.family: theme.fontName
                visible: text.length > 0
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                Layout.fillWidth: true
            }
        }
    }
}
