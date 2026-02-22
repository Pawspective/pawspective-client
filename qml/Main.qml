import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 400
    height: 500
    title: "Login"

    LoginView {
        anchors.fill: parent
    }
}