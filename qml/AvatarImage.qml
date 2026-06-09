import QtQuick 2.15

Item {
    id: root
    property string photoUrl: ""
    property string defaultText: ""
    readonly property string storageBaseUrl: "https://storage.yandexcloud.net/hollow1crown/photos/"

    width: 80
    height: 80

    // Used only to obtain natural image dimensions for center-crop math.
    // visible:false still loads the image and exposes implicitWidth/Height.
    Image {
        id: sizeHelper
        source: root.photoUrl ? (root.storageBaseUrl + root.photoUrl) : ""
        visible: false
        onStatusChanged: {
            if (status === Image.Ready) canvas.requestPaint()
        }
    }

    Canvas {
        id: canvas
        anchors.fill: parent

        onWidthChanged: requestPaint()
        onHeightChanged: requestPaint()

        // Fired when canvas.loadImage() finishes
        onImageLoaded: requestPaint()

        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()

            var r = width / 2

            // Clip everything to a perfect circle
            ctx.beginPath()
            ctx.arc(r, r, r, 0, Math.PI * 2)
            ctx.clip()

            // Background fill (shown when no image or while loading)
            ctx.fillStyle = "#e8d8cb"
            ctx.fillRect(0, 0, width, height)

            var url = root.photoUrl ? (root.storageBaseUrl + root.photoUrl) : ""
            if (url && canvas.isImageLoaded(url) && sizeHelper.status === Image.Ready) {
                var iw = sizeHelper.implicitWidth
                var ih = sizeHelper.implicitHeight
                if (iw > 0 && ih > 0) {
                    // Center-crop: take the largest square from the middle
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

    // Border ring rendered on top of the canvas
    Rectangle {
        anchors.fill: parent
        radius: width / 2
        color: "transparent"
        border.color: "#b8abd7"
        border.width: 1
    }

    // Letter placeholder while image is absent or loading
    Text {
        anchors.centerIn: parent
        text: root.defaultText.length > 0 ? root.defaultText[0].toUpperCase() : "?"
        font.family: "Comic Sans MS"
        font.pixelSize: parent.width * 0.45
        font.bold: true
        color: "#8572af"
        visible: !root.photoUrl || sizeHelper.status !== Image.Ready
    }

    onPhotoUrlChanged: {
        if (photoUrl) {
            canvas.loadImage(root.storageBaseUrl + photoUrl)
        }
        canvas.requestPaint()
    }
}
