import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 1600
    height: 1000
    title: "Edit Profile"

    UserUpdateView {
        anchors.fill: parent
    }
}