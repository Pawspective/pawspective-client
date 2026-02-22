import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15
import pawspective 1.0

ApplicationWindow {
    id: window
    width: 1600
    height: 1000
    visible: true
    title: "User Profile App"

    UserView {
        anchors.fill: parent
    }
}