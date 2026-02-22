import QtQuick 2.15

Item {
    id: root
    width: 120
    height: 120

    property bool running: true

    Image {
        id: spinner
        anchors.fill: parent
        source: "../resources/loading_paw.png"
        fillMode: Image.PreserveAspectFit
        transformOrigin: Item.Center
    }

    RotationAnimation {
        target: spinner
        from: 0
        to: 360
        duration: 1000
        loops : Animation.Infinite
        running: root.running
    }
}