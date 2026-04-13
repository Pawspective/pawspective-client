import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

ApplicationWindow {
    id: window
    width: 1600
    height: 1000
    visible: true
    title: "User Profile App"

    // Session end handling
    Connections {
        target: authService
        function onSessionEnded() {
            if (stackView.depth > 1) {
                stackView.pop()
            }
            sessionExpiredDialog.open()
        }
    }

    Connections {
        target: userUpdateViewModel
        function onSaveCompleted() {
        stackView.pop()
    }
    }

    Connections {
        target: updateOrganizationViewModel
        function onSaveCompleted() {
            stackView.pop()
        }
    }


    function openOrganizationView(organizationId, source) {
        let resolvedOrganizationId = null
        if (organizationId !== null && organizationId !== undefined) {
            const normalizedOrganizationId = Number(organizationId)
            resolvedOrganizationId = Number.isFinite(normalizedOrganizationId) && normalizedOrganizationId > 0
                                   ? normalizedOrganizationId
                                   : null
        }

        const navigationSource = source || "sidebar"
        stackView.replace(null, organizationViewComponent, {
            organizationId: resolvedOrganizationId,
            navigationSource: navigationSource
        })
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
            Component.onDestruction: loginViewModel.cleanup()
        }
    }

    Component {
        id: registerViewComponent
        RegisterView {
            onBackClicked: stackView.pop()
            onRegisterSuccess: {
                stackView.replace(loginViewComponent)
            }
            Component.onDestruction: registerViewModel.cleanup()
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

            onRegisterOrganizationClicked: stackView.push(registerOrganizationViewComponent)
            onOrganizationClicked: function(organizationId) {
                window.openOrganizationView(organizationId, "sidebar")
            }
            onSearchClicked: {
                stackView.replace(null, searchViewComponent, {
                searchOrganizationViewModel: searchOrganizationViewModel
            })
            }

            Component.onCompleted: {
                if (viewModel && viewModel.isAuthenticated) {
                    viewModel.refreshUserData()
                }
            }
        }
    }

    Component {
    id: searchViewComponent
    SearchView {
        searchOrganizationViewModel: searchOrganizationViewModel

        onProfileRequested: stackView.replace(null, userViewComponent)
        onOrganizationClicked: function(organizationId) {
            window.openOrganizationView(organizationId, "sidebar")
        }
        onAnimalDetailRequested: function(animalId) {
            stackView.push(animalDetailViewComponent, { animalId: animalId })
        }

        Component.onCompleted: {
            if (searchOrganizationViewModel) {
                searchOrganizationViewModel.initialize()
            }
            userViewModel = window.userViewModel
        }

        Component.onDestruction: {
            if (searchOrganizationViewModel) {
                searchOrganizationViewModel.cleanup()
            }
        }
    }
}

    Component {
        id: registerOrganizationViewComponent
        RegisterOrganizationView {
            onBackClicked: stackView.pop()
            onRegisterSuccess: stackView.pop()
            Component.onDestruction: registerOrganizationViewModel.cleanup()
        }
    }

    Component {
        id: organizationViewComponent
        OrganizationView {
            onProfileRequested: stackView.replace(null, userViewComponent)
            onSearchRequested: {
                stackView.replace(null, searchViewComponent, {
                    searchOrganizationViewModel: searchOrganizationViewModel
                })
            }
            onOrganizationRequested: function(orgId) { window.openOrganizationView(orgId, "sidebar") }
            
            onCreateOrganizationClicked: stackView.push(registerOrganizationViewComponent)
            onUpdateOrganizationClicked: stackView.push(updateOrganizationViewComponent)
            onCreateAnimalRequested: {
                var orgId = organizationViewModel ? organizationViewModel.currentOrganizationId : 0
                console.log("Creating animal for organization ID:", orgId)
                if (orgId > 0) {
                    createAnimalViewModel.setOrganizationId(orgId)
                }
                stackView.push(animalCreateViewComponent)
            }
            onAnimalDetailRequested: function(animalId) {
                stackView.push(animalDetailViewComponent, { animalId: animalId })
            }
            onBackClicked: stackView.pop()
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
                userUpdateViewModel.saveChanges()
            }
        }
    }

    Component {
        id: updateOrganizationViewComponent
        UpdateOrganizationView {
            viewModel: updateOrganizationViewModel
        onDiscard: {
            updateOrganizationViewModel.cleanup()
            stackView.pop()
        }
        Component.onCompleted: {
            updateOrganizationViewModel.initialize()
        }
        }
    }

    Component {
        id: animalDetailViewComponent
        AnimalDetailView {
            viewModel: animalDetailViewModel
            onBackClicked: stackView.pop()
            onOrganizationRequested: function(orgId) { window.openOrganizationView(orgId, "search") }
            onUpdateAnimalRequested: function(animalId) {
            updateAnimalViewModel.setAnimalId(animalId)
            stackView.push(animalUpdateViewComponent)
        }
        }
    }

    Component {
        id: animalCreateViewComponent
        AnimalCreateView {
            viewModel: createAnimalViewModel

            onBackClicked: {
            createAnimalViewModel.cleanup()
            stackView.pop()
        }

        onCreateSuccess: {
            createAnimalViewModel.cleanup()
            stackView.pop()
        }

        Component.onCompleted: {
            if (createAnimalViewModel) {
                createAnimalViewModel.initialize()
            }
        }
        Component.onDestruction: {
            if (createAnimalViewModel) {
                createAnimalViewModel.cleanup()  
            }
        }
        }
    }
    Component {
    id: animalUpdateViewComponent
    AnimalUpdateView {
        viewModel: updateAnimalViewModel
        
        onDiscard: {
            updateAnimalViewModel.cleanup()
            stackView.pop()
        }
        
        onSaveCompleted: {
            updateAnimalViewModel.cleanup()
            stackView.pop()
        }
        
        Component.onCompleted: {
            if (updateAnimalViewModel) {
                updateAnimalViewModel.initialize()
            }
        }
        Component.onDestruction: {
            if (updateAnimalViewModel) {
                updateAnimalViewModel.cleanup()
            }
        }
    }
}

}
