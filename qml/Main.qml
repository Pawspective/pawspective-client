import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15

ApplicationWindow {
    id: window
    width: 1600
    height: 1000
    visible: true
    title: "User Profile App"
    property bool organizationRefreshPending: false
    property var organizationRefreshHandler: null
    property var organizationRefreshFailHandler: null

    Component.onCompleted: {
        console.log("[Main::Component::onCompleted] APPLICATION STARTED - Main window initialized")
    }

    // Session end handling
    Connections {
        target: authService
        function onSessionEnded() {
            console.warn("[Main::authService::onSessionEnded] SESSION ENDED - User logged out")
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
    signal animalCreated()
    signal animalUpdated()

    function openOrganizationView(organizationId, source, allowRefresh) {
        let resolvedOrganizationId = null
        const shouldRefresh = allowRefresh === undefined ? true : allowRefresh

        if (organizationId !== null && organizationId !== undefined) {
            const normalizedOrganizationId = Number(organizationId)
            resolvedOrganizationId = Number.isFinite(normalizedOrganizationId) && normalizedOrganizationId > 0
                                   ? normalizedOrganizationId
                                   : null
        }

        if (resolvedOrganizationId === null && userViewModel && userViewModel.userData) {
            const fallbackOrgId = Number(userViewModel.userData.organizationId)
            if (Number.isFinite(fallbackOrgId) && fallbackOrgId > 0) {
                resolvedOrganizationId = fallbackOrgId
            }
        }

        if (resolvedOrganizationId === null && userViewModel && shouldRefresh) {
            if (organizationRefreshPending) {
                return
            }

            if (stackView.currentItem && stackView.currentItem.suppressLoading !== undefined) {
                stackView.currentItem.suppressLoading = true
            }

            organizationRefreshPending = true
            organizationRefreshHandler = function() {
                userViewModel.userDataLoaded.disconnect(organizationRefreshHandler)
                if (organizationRefreshFailHandler) {
                    userViewModel.userDataLoadFailed.disconnect(organizationRefreshFailHandler)
                }
                organizationRefreshPending = false
                organizationRefreshHandler = null
                organizationRefreshFailHandler = null
                const loadedOrgId = Number(userViewModel.userData.organizationId)
                openOrganizationView(loadedOrgId, source, false)
            }
            organizationRefreshFailHandler = function() {
                userViewModel.userDataLoaded.disconnect(organizationRefreshHandler)
                userViewModel.userDataLoadFailed.disconnect(organizationRefreshFailHandler)
                organizationRefreshPending = false
                organizationRefreshHandler = null
                organizationRefreshFailHandler = null
                if (stackView.currentItem && stackView.currentItem.suppressLoading !== undefined) {
                    stackView.currentItem.suppressLoading = false
                }
            }

            userViewModel.userDataLoaded.connect(organizationRefreshHandler)
            userViewModel.userDataLoadFailed.connect(organizationRefreshFailHandler)
            userViewModel.refreshUserData()
            return
        }

        const navigationSource = source || "sidebar"
        if (navigationSource == "sidebar") {
            stackView.replace(null, organizationViewComponent, {
                organizationId: resolvedOrganizationId,
                navigationSource: navigationSource
            })
        } else {
            stackView.push(organizationViewComponent, {
                organizationId: resolvedOrganizationId,
                navigationSource: navigationSource
            })
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: loginViewComponent
        
        onCurrentItemChanged: {
            console.debug("[Main::StackView::onCurrentItemChanged] Navigating to:", currentItem ? currentItem.toString() : "null")
        }
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
            onLoginSuccess: {
                console.log("[Main::loginViewComponent::onLoginSuccess] SUCCESS - User logged in, loading UserView")
                try {
                    stackView.replace(userViewComponent)
                    console.debug("[Main::loginViewComponent::onLoginSuccess] StackView.replace() executed")
                } catch (error) {
                    console.error("[Main::loginViewComponent::onLoginSuccess] EXCEPTION during stackView.replace():", error)
                }
            }
            Component.onDestruction: {
                console.debug("[Main::loginViewComponent::onDestruction] Cleaning up LoginView")
                loginViewModel.cleanup()
            }
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

            onLogoutClicked: {
                console.log("[Main::userViewComponent::onLogoutClicked] LOGOUT clicked")
                userViewModel.logout()
                stackView.replace(loginViewComponent)
            }

            onEditProfileClicked: {
                console.debug("[Main::userViewComponent::onEditProfileClicked] Pushing UserUpdateView")
                stackView.push(userUpdateViewComponent)
            }

            onRegisterOrganizationClicked: stackView.push(registerOrganizationViewComponent)
            onOrganizationClicked: function(organizationId) {
                console.debug("[Main::userViewComponent::onOrganizationClicked] Organization clicked:", organizationId)
                window.openOrganizationView(organizationId, "sidebar")
            }
            onSearchClicked: {
                console.debug("[Main::userViewComponent::onSearchClicked] Pushing SearchView")
                stackView.replace(null, searchViewComponent, {
                searchOrganizationViewModel: searchOrganizationViewModel
            })
            }

            Component.onCompleted: {
                console.log("[Main::userViewComponent::Component.onCompleted] UserView LOADED successfully")
                if (viewModel && viewModel.isAuthenticated) {
                    console.log("[Main::userViewComponent::Component.onCompleted] User authenticated, refreshing user data")
                    try {
                        viewModel.refreshUserData()
                    } catch (error) {
                        console.error("[Main::userViewComponent::Component.onCompleted] EXCEPTION during refreshUserData(): " + error)
                    }
                } else {
                    console.warn("[Main::userViewComponent::Component.onCompleted] WARNING - viewModel not authenticated or null")
                }
            }
        }
    }

    Component {
    id: searchViewComponent
    SearchView {
        searchOrganizationViewModel: searchOrganizationViewModel
        currentUserViewModel: userViewModel

        onProfileRequested: stackView.replace(null, userViewComponent)
        onOrganizationClicked: function(organizationId) {
            window.openOrganizationView(organizationId, "search")
        }
        onOrganizationSidebarClicked: function(organizationId) {
            window.openOrganizationView(organizationId, "sidebar")
        }
        onAnimalDetailRequested: function(animalId) {
            stackView.push(animalDetailViewComponent, { animalId: animalId, currentUserViewModel: userViewModel })
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
            onRegisterSuccess: {
                if (!userViewModel) {
                    stackView.pop()
                    return
                }
                window.openOrganizationView(null, "sidebar", true)
            }
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
                stackView.push(animalDetailViewComponent, { animalId: animalId, currentUserViewModel: userViewModel })
            }
            onBackClicked: stackView.pop()

            Connections {
            target: window
            function onAnimalCreated() {
                if (organizationViewModel) {
                    var orgId = organizationViewModel.currentOrganizationId
                    if (orgId > 0 && animalListViewModel) {
                        console.log("Animal created, reloading animals for org:", orgId)
                        animalListViewModel.loadAnimalsForOrganization(orgId)
                    }
                }
            }
            function onAnimalUpdated() {
                if (organizationViewModel) {
                    var orgId = organizationViewModel.currentOrganizationId
                    if (orgId > 0 && animalListViewModel) {
                        console.log("Animal updated, reloading animals for org:", orgId)
                        animalListViewModel.loadAnimalsForOrganization(orgId)
                    }
                }
            }
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
            currentUserViewModel: userViewModel
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
            animalCreated()
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
            window.animalUpdated()
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
