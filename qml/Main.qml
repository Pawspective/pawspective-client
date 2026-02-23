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

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: loginViewComponent
    }

    Component {
        id: loginViewComponent
        LoginView {
            anchors.fill: parent
            Component.onCompleted: {
                var loginButton = findChild(button => 
                    button instanceof Button && button.text === "Login")
                
                if (loginButton) {
                    var originalClick = loginButton.clicked
                    
                    loginButton.clicked.connect(function() {
                        originalClick()
                        
                        var timer = Qt.createQmlObject(
                            "import QtQuick 2.0; Timer { interval: 2100; running: true; repeat: false; }",
                            loginButton
                        )
                        timer.triggered.connect(function() {
                            stackView.push(userViewComponent)
                        })
                    })
                }
            }

            function findChild(matchFunc) {
                for (var i = 0; i < children.length; i++) {
                    var child = children[i]
                    if (matchFunc(child)) return child
                    
                    if (child.children) {
                        var found = findChildRecursive(child, matchFunc)
                        if (found) return found
                    }
                }
                return null
            }

            function findChildRecursive(parent, matchFunc) {
                for (var i = 0; i < parent.children.length; i++) {
                    var child = parent.children[i]
                    if (matchFunc(child)) return child
                    
                    if (child.children) {
                        var found = findChildRecursive(child, matchFunc)
                        if (found) return found
                    }
                }
                return null
            }
        }
    }

    Component {
        id: userViewComponent
        UserView {
        anchors.fill: parent  
    }
    }
}