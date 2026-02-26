import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

ApplicationWindow {
    id: window
    width: 1600
    height: 1000
    visible: true
    title: "User Profile App"

    StackView {
        id: stackView
        anchors.fill: parent
        // Устанавливаем начальное окно
        initialItem: loginViewComponent
    }

    // --- КОМПОНЕНТЫ ЭКРАНОВ ---

    Component {
        id: loginViewComponent
        LoginView {
            // Когда в LoginView нажали "Register"
            onRegisterRequested: stackView.push(registerViewComponent)
            
            // Когда логин прошел успешно (после таймера внутри LoginView)
            onLoginSuccess: stackView.replace(userViewComponent)
        }
    }

    Component {
        id: registerViewComponent
        RegisterView {
            // Та самая стрелочка назад, которую мы добавили
            onBackClicked: stackView.pop()
            
            // Если регистрация прошла успешно
            onRegisterSuccess: stackView.replace(userViewComponent)
        }
    }

    Component {
        id: userViewComponent
        UserView {
            // Сигнал выхода
            onLogoutClicked: stackView.replace(loginViewComponent)
        onEditProfileClicked: stackView.push(userUpdateViewComponent, {
                userEmail: userEmail,
                userFirstName: userFirstName,
                userLastName: userLastName
            })
        }
    }

    Component {
        id: userUpdateViewComponent
        UserUpdateView {
            onDiscard: stackView.pop()
            onSubmit: {
                // Здесь можно добавить логику сохранения данных
                console.log("Saving changes...")
                stackView.pop()
            }
        }
    }
}