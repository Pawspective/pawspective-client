import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15
import Pawspective.ViewModels 1.0

ApplicationWindow {
    id: window
    width: 1600
    height: 1000
    visible: true
    title: "User Profile App"

    // ViewModels
    UserViewModel {
        id: userViewModel
        onSessionExpired: {
            sessionExpiredDialog.open()
        }
    }

    UserUpdateViewModel {
        id: userUpdateViewModel
        onSessionExpired: {
            stackView.pop()
            sessionExpiredDialog.open()
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: loginViewComponent
    }

    // Session expired dialog
    Dialog {
        id: sessionExpiredDialog
        title: "Session Expired"
        standardButtons: Dialog.Ok
        modal: true
        anchors.centerIn: parent
        width: 400
        height: 150

        contentItem: Text {
            text: "Your session has expired. Please log in again."
            wrapMode: Text.WordWrap
            anchors.fill: parent
            anchors.margins: 20
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 14
        }

        onAccepted: {
            stackView.replace(loginViewComponent)
        }
    }

    // --- SCREEN COMPONENTS ---

    Component {
        id: loginViewComponent
        LoginView {
            onRegisterRequested: stackView.push(registerViewComponent)
            onLoginSuccess: stackView.replace(userViewComponent)
        }
    }

    Component {
        id: registerViewComponent
        RegisterView {
            onBackClicked: stackView.pop()
            onRegisterSuccess: {
                stackView.replace(userViewComponent)
            }
        }
    }

    Component {
        id: userViewComponent
        UserView {
            viewModel: userViewModel
            onLogoutClicked: stackView.replace(loginViewComponent)
            onEditProfileClicked: {
                userUpdateViewModel.initialize()
                stackView.push(userUpdateViewComponent)
            }
        }
    }

    Component {
        id: userUpdateViewComponent
        UserUpdateView {
            viewModel: userUpdateViewModel
            onDiscard: {
                userUpdateViewModel.cleanup()
                stackView.pop()
            }
            onSubmit: {
                // Wait for result
            }
        }
    }
}