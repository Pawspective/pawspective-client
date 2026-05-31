import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property var viewModel: null
    property int animalId: 0
    property var currentUserViewModel: userViewModel

    signal backClicked()
    signal organizationRequested(int organizationId)
    signal updateAnimalRequested(int animalId)
    signal animalDeleted()

    property bool requestJustSent: false

    Connections {
        target: root.viewModel
        function onErrorOccurred(type, message) {
            errorText.text = message
        }

        function onDeleteSuccess() {
            errorText.text = ""
            root.animalDeleted()
        }
        function onDeleteFailed(message) {
            errorText.text = message
        }
        function onAdoptSuccess() {
            errorText.text = ""
            root.requestJustSent = true
        }
        function onAdoptFailed(message) {
            errorText.text = message
        }
    }

    StackView.onActivated: {
        if (root.viewModel && root.animalId > 0) {
            root.viewModel.loadAnimal(root.animalId)
        }
    }

    onAnimalIdChanged: {
        if (root.viewModel && root.animalId > 0) {
            root.viewModel.loadAnimal(root.animalId)
        }
    }

    color: "#e8d8cb"

    QtObject {
        id: theme
        readonly property string fontName: "Comic Sans MS"
        readonly property color fieldBg: "#fdfdfd"
        readonly property color purple: "#b8abd7"
        readonly property color textDark: "#8572af"
        readonly property color accentPink: "#f4a7b9"
        readonly property color buttonText: "#e7ebf5"
        readonly property color chipBg: "#e8d8cb"
        readonly property color border: "#b8abd7"
        readonly property color deleteRed: "#e26d5c"
    }

    readonly property real fieldLabelSize: root.height * 0.022
    readonly property real fieldValueSize: root.height * 0.025
    readonly property real fieldHeight: root.height * 0.06
    readonly property real fieldSpacing: root.height * 0.008
    readonly property real contentSpacing: root.height * 0.02
    readonly property real sideMargin: root.width * 0.05

    readonly property bool isOwnOrganization: {
        if (!viewModel || !root.currentUserViewModel) return false
        var userOrgId = root.currentUserViewModel.userData ? Number(root.currentUserViewModel.userData.organizationId) : 0
        var animalOrgId = Number(viewModel.organizationId)
        return userOrgId > 0 && animalOrgId > 0 && userOrgId === animalOrgId
    }

    Rectangle {
        id: backButton
        z: 10
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: root.height * 0.02
        anchors.leftMargin: root.sideMargin
        width: root.height * 0.06
        height: root.height * 0.06
        radius: width / 2
        color: backArea.containsMouse ? theme.accentPink : theme.purple

        Text {
            anchors.centerIn: parent
            text: "←"
            font.pixelSize: parent.height * 0.45
            color: theme.buttonText
        }

        MouseArea {
            id: backArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: root.backClicked()
        }
    }

    LoaderSpinner {
        anchors.centerIn: parent
        running: root.viewModel ? root.viewModel.isBusy : false
        visible: root.viewModel ? root.viewModel.isBusy : false
        z: 10
    }

    Text {
        id: errorText
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: root.height * 0.04
        visible: text.length > 0
        color: theme.textDark
        font.family: theme.fontName
        font.pixelSize: root.fieldLabelSize
        z: 10
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent
        clip: true
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

        ColumnLayout {
            width: scrollView.width
            spacing: root.contentSpacing

            Item { Layout.preferredHeight: root.height * 0.09 }

            Rectangle {
                Layout.fillWidth: true
                Layout.leftMargin: root.sideMargin
                Layout.rightMargin: root.sideMargin
                implicitHeight: headerContent.implicitHeight + root.height * 0.04
                radius: 16
                color: theme.fieldBg
                border.color: theme.border
                border.width: 1

                ColumnLayout {
                    id: headerContent
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                        margins: root.height * 0.02
                    }
                    spacing: root.height * 0.015

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: root.width * 0.025

                        Rectangle {
                            width: root.height * 0.14
                            height: root.height * 0.14
                            radius: width / 2
                            color: theme.chipBg
                            border.color: theme.border
                            border.width: 1
                            Layout.alignment: Qt.AlignVCenter

                            Text {
                                anchors.centerIn: parent
                                text: (root.viewModel && root.viewModel.animalType && root.viewModel.animalType.length > 0)
                                    ? root.viewModel.animalType[0].toUpperCase() : "?"
                                font.family: theme.fontName
                                font.pixelSize: parent.width * 0.4
                                font.bold: true
                                color: theme.textDark
                            }
                        }

                        ColumnLayout {
                            spacing: root.height * 0.008
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft

                            Text {
                                text: root.viewModel ? root.viewModel.name : ""
                                font.family: theme.fontName
                                font.pixelSize: root.height * 0.048
                                font.bold: true
                                color: theme.textDark
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }

                            Text {
                                visible: root.viewModel && root.viewModel.description ? root.viewModel.description.length > 0 : false
                                text: root.viewModel && root.viewModel.description ? root.viewModel.description : ""
                                font.family: theme.fontName
                                font.pixelSize: root.height * 0.028
                                color: theme.textDark
                                wrapMode: Text.WordWrap
                                Layout.fillWidth: true
                            }
                        }
                    }
                }
            }

            DetailField {
                label: "Type"
                value: root.viewModel ? root.viewModel.animalType : ""
            }
            DetailField {
                label: "Breed"
                value: root.viewModel ? root.viewModel.breedName : ""
            }
            DetailField {
                label: "Age"
                value: root.viewModel ? root.viewModel.age + " y.o." : ""
            }
            DetailField {
                label: "Gender"
                value: root.viewModel ? root.viewModel.gender : ""
            }
            DetailField {
                label: "Size"
                value: root.viewModel ? root.viewModel.size : ""
            }
            DetailField {
                label: "Care Level"
                value: root.viewModel ? root.viewModel.careLevel : ""
            }
            DetailField {
                label: "Color"
                value: root.viewModel ? root.viewModel.color : ""
            }
            DetailField {
                label: "Good With"
                value: root.viewModel ? root.viewModel.goodWith : ""
            }
            DetailField {
                label: "Status"
                value: root.viewModel ? root.viewModel.status : ""
            }

            OrganizationCardView {
                visible: !root.isOwnOrganization && root.viewModel && (typeof root.viewModel.organizationId !== 'undefined')
                    && root.viewModel.organizationId > 0
                organizationId: (root.viewModel && typeof root.viewModel.organizationId !== 'undefined')
                    ? root.viewModel.organizationId : 0
                organizationName: (root.viewModel && root.viewModel.organizationName) ? root.viewModel.organizationName : ""
                organizationCity: (root.viewModel && root.viewModel.organizationCity) ? root.viewModel.organizationCity : ""
                organizationDescription: (root.viewModel && root.viewModel.organizationDescription)
                    ? root.viewModel.organizationDescription : ""
                Layout.fillWidth: true
                Layout.leftMargin: root.sideMargin
                Layout.rightMargin: root.sideMargin
                onClicked: function(id) { root.organizationRequested(id) }
            }

            RowLayout {
                visible: !root.isOwnOrganization && root.viewModel && !root.viewModel.isBusy
                Layout.fillWidth: true
                Layout.leftMargin: root.sideMargin
                Layout.rightMargin: root.sideMargin
                Layout.preferredHeight: root.height * 0.08

                CustomButton {
                    readonly property bool canAdopt: root.viewModel && root.viewModel.canBeAdopted
                    readonly property string disabledReason: {
                        if (!root.viewModel) return ""
                        var s = root.viewModel.status
                        if (s === "adopted") return "already adopted"
                        return "request already sent or animal is unavailable"
                    }

                    text: root.requestJustSent ? "Request sent"
                        : canAdopt ? "Adopt"
                        : "You can't adopt now (" + disabledReason + ")"
                    enabled: canAdopt && !root.requestJustSent
                    baseColor: (canAdopt && !root.requestJustSent) ? theme.purple : "#b0b0b0"
                    hoverColor: (canAdopt && !root.requestJustSent) ? theme.accentPink : "#b0b0b0"
                    textColor: theme.buttonText
                    fontSize: root.height * 0.025
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    onClicked: {
                        if (root.viewModel) root.viewModel.adoptAnimal()
                    }
                }
            }

            RowLayout {
                visible: root.isOwnOrganization
                Layout.fillWidth: true
                Layout.leftMargin: root.sideMargin
                Layout.rightMargin: root.sideMargin
                Layout.preferredHeight: root.height * 0.08
                spacing: root.width * 0.03

                CustomButton {
                    text: "Update Animal"
                    baseColor: theme.purple
                    hoverColor: theme.accentPink
                    textColor: theme.buttonText
                    fontSize: root.height * 0.025
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    onClicked: {
                        console.log("Update animal clicked, id:", root.animalId)
                        root.updateAnimalRequested(root.animalId)
                    }
                }

                CustomButton {
                    text: "Delete Animal"
                    baseColor: theme.deleteRed
                    hoverColor: theme.accentPink
                    textColor: theme.buttonText
                    fontSize: root.height * 0.025
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    onClicked: {
                        console.log("Delete animal clicked, id:", root.animalId)
                        deleteConfirmDialog.open()
                    }
                }
            }

            Item { Layout.preferredHeight: root.height * 0.05 }
        }
    }

    component DetailField : ColumnLayout {
        property string label: ""
        property string value: ""

        Layout.fillWidth: true
        Layout.leftMargin: root.sideMargin
        Layout.rightMargin: root.sideMargin
        spacing: root.fieldSpacing

        Text {
            text: parent.label
            font.family: theme.fontName
            font.pixelSize: root.fieldLabelSize
            color: theme.textDark
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: root.fieldHeight
            color: theme.fieldBg
            radius: 10

            Text {
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                text: parent.parent.value ? parent.parent.value : ""
                font.family: theme.fontName
                font.pixelSize: root.fieldValueSize
                color: theme.accentPink
            }
        }
    }

    Dialog {
        id: deleteConfirmDialog
        modal: true
        parent: ApplicationWindow.overlay
        anchors.centerIn: parent
        width: parent.width * 0.8
        title: "Delete Animal Profile"
        standardButtons: Dialog.Yes | Dialog.No

        onAccepted: {
            if (viewModel) viewModel.deleteAnimal()
        }

        contentItem: Text {
            text: "Are you sure you want to permanently delete this animal?"
            wrapMode: Text.WordWrap
            anchors.fill: parent
            anchors.margins: 20
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 14
        }
    }

}
