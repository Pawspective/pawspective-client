import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

Rectangle {
    id: root
    anchors.fill: parent
    color: "#e8d8cb"

    property var viewModel: null
    property var uploaderViewModel: null
    property int postId: 0
    property string postText: ""
    property var postCreatedAt: null
    property var postPhotos: []
    property string errorMessage: ""

    signal backClicked()
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
    }

    readonly property real contentMargins: root.height * 0.05
    readonly property real contentSpacing: root.height * 0.02
    readonly property real fieldLabelFontSize: root.height * 0.022
    readonly property real fieldValueFontSize: root.height * 0.025
    readonly property real buttonHeight: root.height * 0.08
    readonly property real buttonFontSize: root.height * 0.025
    readonly property real buttonSpacing: root.height * 0.02
    readonly property real loaderSize: root.height * 0.1
    readonly property real photoThumbSize: root.height * 0.13
    readonly property real uploaderPreviewSize: root.height * 0.22
    readonly property real bottomPadding: root.height * 0.05
    
    readonly property real avatarSize: root.width * 0.09

    Connections {
        target: viewModel
        function onErrorOccurred(type, message) {
            root.errorMessage = message
            errorTimer.start()
        }
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
        if (viewModel && postId > 0) {
            viewModel.setPostData(postId, postText, postCreatedAt, postPhotos || [])
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
                text: "Update Post"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                font.family: theme.fontName
                font.pixelSize: root.height * 0.05
                font.bold: true
                color: theme.textDark
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: root.height * 0.01
                Layout.leftMargin: root.width * 0.05
                Layout.rightMargin: root.width * 0.05
                
                Text {
                    text: "Post Content *"
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
                            id: textInput
                            text: viewModel ? viewModel.text : ""
                            font.family: theme.fontName
                            font.pixelSize: root.fieldValueFontSize
                            color: theme.accentPink
                            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                            leftPadding: 10
                            topPadding: 10
                            enabled: viewModel && !viewModel.isBusy
                            onTextChanged: {
                                if (focus && viewModel) {
                                    viewModel.text = text
                                }
                            }
                            background: null
                            placeholderText: "Write your post here..."
                            placeholderTextColor: theme.accentPink
                        }
                    }
                }

                Text {
                    text: (viewModel ? viewModel.text.length : 0) + " / 2000 characters"
                    font.family: theme.fontName
                    font.pixelSize: root.fieldLabelFontSize
                    color: (viewModel && viewModel.text.length > 2000) ? theme.errorColor : theme.textDark
                    Layout.alignment: Qt.AlignRight
                    Layout.fillWidth: true
                }
            }
            
            ColumnLayout {
                Layout.fillWidth: true
                spacing: root.height * 0.01
                Layout.leftMargin: root.width * 0.05
                Layout.rightMargin: root.width * 0.05

                Text {
                    text: "Photos"
                    font.family: theme.fontName
                    font.pixelSize: root.fieldLabelFontSize
                    color: theme.textDark
                }

                Flow {
                    Layout.fillWidth: true
                    spacing: 8
                    visible: viewModel && viewModel.photos.length > 0

                    Repeater {
                        model: viewModel ? viewModel.photos : []
                        delegate: Item {
                            width: root.photoThumbSize + 10
                            height: root.photoThumbSize + 10

                            Rectangle {
                                width: root.photoThumbSize
                                height: root.photoThumbSize
                                radius: 8
                                clip: true
                                anchors.centerIn: parent
                                color: "#e8d8cb"

                                Image {
                                    anchors.fill: parent
                                    source: AppConfig.storageBaseUrl + modelData
                                    fillMode: Image.PreserveAspectCrop
                                }
                            }

                            Rectangle {
                                width: 20; height: 20; radius: 10
                                color: "#ff6b6b"
                                anchors.top: parent.top
                                anchors.right: parent.right

                                Text {
                                    anchors.centerIn: parent
                                    text: "×"
                                    color: "white"
                                    font.pixelSize: 13
                                    font.bold: true
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: { if (viewModel) viewModel.removePhoto(modelData) }
                                }
                            }
                        }
                    }
                }

                PhotoUploader {
                    Layout.fillWidth: true
                    title: ""
                    viewModel: root.uploaderViewModel
                    previewSize: root.uploaderPreviewSize
                    previewRadius: 8
                    buttonHeight: root.buttonHeight
                    buttonFontSize: root.buttonFontSize
                    onUploadCompleted: function(fileName) {
                        updatePostViewModel.addPhoto(fileName)
                    }
                }
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
                        root.backClicked()
                    }
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
            }

            Item { Layout.preferredHeight: root.bottomPadding }
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