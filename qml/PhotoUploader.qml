import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs

Rectangle {
    id: root

    property var viewModel: null
    property string title: "Upload Photo"

    property real previewSize: Math.min(root.width * 0.15, 80)
    property int previewAlignment: Qt.AlignHCenter
    property real previewRadius: root.previewSize / 2
    property real buttonHeight: 48
    property real buttonFontSize: 15

    implicitHeight: contentLayout.implicitHeight

    signal uploadCompleted(string fileName)

    color: "transparent"
    property string selectedFilePath: ""
    property url previewUrl: ""
    onPreviewUrlChanged: {
        if (previewUrl && previewCanvas) {
            previewCanvas.loadImage(previewUrl)
        }
        if (previewCanvas) previewCanvas.requestPaint()
    }
    
    function cleanFilePath(fileUrl) {
        var path = fileUrl.toString()
        if (path.startsWith("file:///")) {
            path = path.substring(8)
        } else if (path.startsWith("file://")) {
            path = path.substring(7)
        }
        if (path.startsWith("/") && path.length > 1 && path[1] == ':') {
            path = path.substring(1)
        }

        path = decodeURIComponent(path)
        
        return path
    }
    
    function hasValidViewModel() {
        return viewModel !== null && viewModel !== undefined
    }
    
    Connections {
        id: viewModelConnections
        target: null
        ignoreUnknownSignals: true
        
        function onUploadCompleted(fileName) {
            root.selectedFilePath = ""
            root.uploadCompleted(fileName)
        }
        
        function onUploadFailed(errorMessage) {
            console.log("Upload failed:", errorMessage)
        }
    }
    
    onViewModelChanged: {
        viewModelConnections.target = hasValidViewModel() ? viewModel : null
        console.log("ViewModel changed, target set to:", viewModelConnections.target)
    }
    
    Component.onCompleted: {
        viewModelConnections.target = hasValidViewModel() ? viewModel : null
        console.log("PhotoUploader completed, viewModel =", viewModel)
    }
    
    function uploadPhoto() {
        console.log("uploadPhoto() called, selectedFilePath =", selectedFilePath)
        
        if (selectedFilePath === "") {
            console.log("No file selected")
            return
        }
        if (!hasValidViewModel()) {
            console.log("No valid viewModel")
            return
        }
        
        console.log("Calling viewModel.uploadPhoto with path:", selectedFilePath)
        viewModel.uploadPhoto(selectedFilePath)
    }
    
    ColumnLayout {
        id: contentLayout
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        spacing: 12
        
        Text {
            text: root.title
            font.family: "Comic Sans MS"
            font.pixelSize: 16
            font.bold: true
            color: "#8572af"
            visible: root.title.length > 0
        }
        
        Item {
            Layout.preferredWidth: root.previewSize
            Layout.preferredHeight: root.previewSize
            Layout.maximumWidth: root.previewSize
            Layout.maximumHeight: root.previewSize
            Layout.alignment: root.previewAlignment

            // Hidden Image used to obtain natural image dimensions
            Image {
                id: previewSizeHelper
                source: root.previewUrl
                visible: false
                onStatusChanged: {
                    if (status === Image.Ready) previewCanvas.requestPaint()
                }
            }

            Canvas {
                id: previewCanvas
                anchors.fill: parent

                onWidthChanged: requestPaint()
                onHeightChanged: requestPaint()

                onImageLoaded: requestPaint()

                onPaint: {
                    var ctx = getContext("2d")
                    ctx.reset()

                    // Background fill
                    ctx.fillStyle = "#f0ecf9"
                    ctx.fillRect(0, 0, width, height)

                    var url = root.previewUrl ? root.previewUrl : ""
                    if (url && previewCanvas.isImageLoaded(url) && previewSizeHelper.status === Image.Ready) {
                        var iw = previewSizeHelper.implicitWidth
                        var ih = previewSizeHelper.implicitHeight
                        if (iw > 0 && ih > 0) {
                            var side = Math.min(iw, ih)
                            var sx = (iw - side) / 2
                            var sy = (ih - side) / 2
                            ctx.drawImage(url, sx, sy, side, side, 0, 0, width, height)
                        } else {
                            ctx.drawImage(url, 0, 0, width, height)
                        }
                    }
                }
            }

            // Border on top (square)
            Rectangle {
                anchors.fill: parent
                radius: 0
                color: "transparent"
                border.color: "#b8abd7"
                border.width: 1
            }

            // Placeholder text
            Text {
                anchors.centerIn: parent
                text: "No\nPhoto"
                font.family: "Comic Sans MS"
                font.pixelSize: 14
                color: "#b8abd7"
                horizontalAlignment: Text.AlignHCenter
                visible: !root.previewUrl || previewSizeHelper.status !== Image.Ready
            }
        }
        
        RowLayout {
            Layout.fillWidth: true
            spacing: 12
            
            CustomButton {
                text: "Select Photo"
                baseColor: "#b8abd7"
                hoverColor: "#f4a7b9"
                textColor: "white"
                fontSize: root.buttonFontSize
                Layout.fillWidth: true
                Layout.preferredHeight: root.buttonHeight
                onClicked: fileDialog.open()
            }

            CustomButton {
                text: (hasValidViewModel() && viewModel && viewModel.isUploading) ? "Uploading..." : "Upload"
                baseColor: selectedFilePath !== "" ? "#b8abd7" : "#cccccc"
                hoverColor: "#f4a7b9"
                textColor: "white"
                fontSize: root.buttonFontSize
                Layout.fillWidth: true
                Layout.preferredHeight: root.buttonHeight
                enabled: selectedFilePath !== "" && hasValidViewModel() && viewModel && !viewModel.isUploading
                onClicked: uploadPhoto()
            }
        }
        
        Text {
            text: hasValidViewModel() && viewModel ? (viewModel.errorMessage || "") : ""
            color: "#ff6b6b"
            font.pixelSize: 11
            visible: text !== ""
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
        }
        
        LoaderSpinner {
            Layout.alignment: Qt.AlignHCenter
            running: hasValidViewModel() && viewModel ? viewModel.isUploading : false
            visible: hasValidViewModel() && viewModel ? viewModel.isUploading : false
        }
    }
    
    FileDialog {
        id: fileDialog
        title: "Select Photo"
        nameFilters: ["Images (*.png *.jpg *.jpeg *.webp *.gif)"]
        
        onAccepted: {
            if (fileDialog.selectedFile) {
                selectedFilePath = cleanFilePath(fileDialog.selectedFile)
                previewUrl = fileDialog.selectedFile
                console.log("File selected (cleaned):", selectedFilePath)
            }
        }
    }
}