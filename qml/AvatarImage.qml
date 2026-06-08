import QtQuick 2.15

Rectangle {
    id: root
    property string photoUrl: ""
    property string defaultText: ""
    property string storageBaseUrl: "https://storage.yandexcloud.net/hollow1crown/photos/"
    
    width: 80
    height: 80
    radius: width / 2
    color: "#e8d8cb"
    border.color: "#b8abd7"
    border.width: 1
    clip: true
    
    Image {
        id: avatarImage
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        smooth: true
        source: root.photoUrl ? root.storageBaseUrl + root.photoUrl : ""
    }
    onPhotoUrlChanged: {
        var oldSource = avatarImage.source
        avatarImage.source = ""
        avatarImage.source = oldSource
    }
    
    Text {
        anchors.centerIn: parent
        text: root.defaultText.length > 0 ? root.defaultText[0].toUpperCase() : "?"
        font.pixelSize: parent.width * 0.45
        font.bold: true
        color: "#8572af"
        visible: avatarImage.status !== Image.Ready
    }
}