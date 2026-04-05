import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property var viewModel: null

    signal backClicked()

    color: "#e8d8cb"

    QtObject {
        id: theme
        readonly property string fontName: "Comic Sans MS"
        readonly property color cardBg: "#fdfdfd"
        readonly property color purple: "#8572af"
        readonly property color pink: "#f4a7b9"
        readonly property color border: "#b8abd7"
        readonly property color chipBg: "#e8d8cb"
    }

    readonly property real pad: root.height * 0.04
    readonly property real labelSize: root.height * 0.025
    readonly property real valueSize: root.height * 0.028
    readonly property real titleSize: root.height * 0.048

    ScrollView {
        anchors.fill: parent
        contentWidth: root.width
        clip: true

        ColumnLayout {
            width: root.width - root.pad * 2
            spacing: root.height * 0.025

            // Header
            RowLayout {
                Layout.fillWidth: true

                CustomButton {
                    text: "← Back"
                    baseColor: theme.border
                    hoverColor: theme.pink
                    textColor: theme.cardBg
                    fontSize: root.labelSize
                    Layout.preferredWidth: root.width * 0.1
                    Layout.preferredHeight: root.height * 0.055
                    onClicked: root.backClicked()
                }

                Item { Layout.fillWidth: true }
            }

            // Main card
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: cardContent.implicitHeight + root.pad * 2
                radius: root.width * 0.012
                color: theme.cardBg
                border.color: theme.border
                border.width: 1

                ColumnLayout {
                    id: cardContent
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                        margins: root.pad
                    }
                    spacing: root.height * 0.02

                    // Avatar + name
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

                            Text {
                                anchors.centerIn: parent
                                text: root.viewModel && root.viewModel.animalType.length > 0
                                    ? root.viewModel.animalType[0].toUpperCase() : "?"
                                font.family: theme.fontName
                                font.pixelSize: parent.width * 0.4
                                font.bold: true
                                color: theme.purple
                            }
                        }

                        ColumnLayout {
                            spacing: root.height * 0.008

                            Text {
                                text: root.viewModel ? root.viewModel.name : ""
                                font.family: theme.fontName
                                font.pixelSize: root.titleSize
                                font.bold: true
                                color: theme.purple
                            }

                            Text {
                                text: root.viewModel
                                    ? root.viewModel.animalType + " · " + root.viewModel.breedName
                                    : ""
                                font.family: theme.fontName
                                font.pixelSize: root.labelSize
                                color: theme.pink
                            }
                        }
                    }

                    // Divider
                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: theme.border
                        opacity: 0.5
                    }

                    // Info grid
                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        rowSpacing: root.height * 0.015
                        columnSpacing: root.width * 0.04

                        Repeater {
                            model: [
                                { label: "Age",        value: root.viewModel ? root.viewModel.age + " y.o." : "" },
                                { label: "Gender",     value: root.viewModel ? root.viewModel.gender : "" },
                                { label: "Size",       value: root.viewModel ? root.viewModel.size : "" },
                                { label: "Care level", value: root.viewModel ? root.viewModel.careLevel : "" },
                                { label: "Color",      value: root.viewModel ? root.viewModel.color : "" },
                                { label: "Good with",  value: root.viewModel ? root.viewModel.goodWith : "" },
                                { label: "Status",     value: root.viewModel ? root.viewModel.status : "" }
                            ]

                            ColumnLayout {
                                spacing: root.height * 0.004

                                Text {
                                    text: modelData.label
                                    font.family: theme.fontName
                                    font.pixelSize: root.labelSize * 0.85
                                    color: theme.pink
                                    opacity: 0.85
                                }

                                Text {
                                    text: modelData.value
                                    font.family: theme.fontName
                                    font.pixelSize: root.valueSize
                                    font.bold: true
                                    color: theme.purple
                                }
                            }
                        }
                    }

                    // Description
                    ColumnLayout {
                        Layout.fillWidth: true
                        visible: root.viewModel && root.viewModel.description.length > 0
                        spacing: root.height * 0.006

                        Rectangle {
                            Layout.fillWidth: true
                            height: 1
                            color: theme.border
                            opacity: 0.5
                        }

                        Text {
                            text: "Description"
                            font.family: theme.fontName
                            font.pixelSize: root.labelSize * 0.85
                            color: theme.pink
                            opacity: 0.85
                        }

                        Text {
                            text: root.viewModel ? root.viewModel.description : ""
                            font.family: theme.fontName
                            font.pixelSize: root.valueSize
                            color: theme.purple
                            wrapMode: Text.WordWrap
                            Layout.fillWidth: true
                        }
                    }
                }
            }
        }
    }
}
