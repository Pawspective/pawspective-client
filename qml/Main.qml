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
        userViewModel.initialize()
    }

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
        target: userViewModel
        function onSessionRestored() {
            stackView.replace(userViewComponent)
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
    signal animalDeleted()
    signal reviewCreated()
    signal reviewUpdated()
    signal postCreated()
    signal postUpdated()

    function openOrganizationView(organizationId, source, allowRefresh, ignoreFallback) {
        let resolvedOrganizationId = null
        const shouldRefresh = allowRefresh === undefined ? true : allowRefresh
        const skipCache = ignoreFallback === undefined ? false : ignoreFallback

        if (organizationId !== null && organizationId !== undefined) {
            const normalizedOrganizationId = Number(organizationId)
            resolvedOrganizationId = Number.isFinite(normalizedOrganizationId) && normalizedOrganizationId > 0
                                   ? normalizedOrganizationId
                                   : null
        }

        if (resolvedOrganizationId === null && userViewModel && userViewModel.userData && !skipCache) {
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

            onLogoutClicked: {
                userViewModel.logout()
                stackView.replace(loginViewComponent)
            }

            onEditProfileClicked: {
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
            onCreateReviewRequested: {
                stackView.push(reviewCreateViewComponent)
            }
            onReviewEditRequested: function(reviewId, reviewText) {
                stackView.push(reviewUpdateViewComponent, {
                    reviewId: reviewId,
                    reviewText: reviewText
                })
            }
            onCreateAnimalRequested: {
                var orgId = organizationViewModel ? organizationViewModel.currentOrganizationId : 0
                console.log("Creating animal for organization ID:", orgId)
                if (orgId > 0) {
                    createAnimalViewModel.setOrganizationId(orgId)
                }
                stackView.push(animalCreateViewComponent)
            }
            onCreatePostRequested: {   
                var orgId = organizationViewModel ? organizationViewModel.currentOrganizationId : 0
                console.log("Creating post for organization ID:", orgId)
                if (orgId > 0) {
                    createPostViewModel.setOrganizationId(orgId)
                }
                stackView.push(postCreateViewComponent)
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
            function onAnimalDeleted() {
                if (organizationViewModel) {
                    var orgId = organizationViewModel.currentOrganizationId
                    if (orgId > 0 && animalListViewModel) {
                        console.log("Animal deleted, reloading animals for org:", orgId)
                        animalListViewModel.loadAnimalsForOrganization(orgId)
                    }
                }
            }
            function onPostCreated() {
                if (organizationViewModel && typeof postListViewModel !== 'undefined') {
                    var orgId = organizationViewModel.currentOrganizationId
                    if (orgId > 0) {
                        console.log("Post created, reloading posts for org:", orgId)
                        postListViewModel.loadPostsForOrganization(orgId)
                    }
                }
            }
            function onPostUpdated() {
                if (organizationViewModel && typeof postListViewModel !== 'undefined') {
                    var orgId = organizationViewModel.currentOrganizationId
                    if (orgId > 0) {
                        console.log("Post updated, reloading posts for org:", orgId)
                        postListViewModel.loadPostsForOrganization(orgId)
                    }
                }
            }
            function onReviewCreated() {
                if (organizationViewModel && typeof reviewListViewModel !== 'undefined') {
                    var orgId = organizationViewModel.currentOrganizationId
                    if (orgId > 0) {
                        console.log("Review created, reloading reviews for org:", orgId)
                        reviewListViewModel.loadReviewsForOrganization(orgId)
                    }
                }
            }
            function onReviewUpdated() {
                if (organizationViewModel && typeof reviewListViewModel !== 'undefined') {
                    var orgId = organizationViewModel.currentOrganizationId
                    if (orgId > 0) {
                        console.log("Review updated, reloading reviews for org:", orgId)
                        reviewListViewModel.loadReviewsForOrganization(orgId)
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
            onAccountDeleted: {
            userUpdateViewModel.cleanup()
            stackView.replace(loginViewComponent) 
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
        onOrganizationDeleted: {
            updateOrganizationViewModel.cleanup()
            stackView.pop()
            window.openOrganizationView(null, "sidebar", true, true) 
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
            onAnimalDeleted: function() {
                stackView.pop()
                window.animalDeleted() 
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
        id: postCreateViewComponent
        PostCreateView {
            viewModel: createPostViewModel
            
            onBackClicked: {
                createPostViewModel.cleanup()
                stackView.pop()
            }
            
            onCreateSuccess: {
                createPostViewModel.cleanup()
                stackView.pop()
                window.postCreated()
            }
            
            Component.onCompleted: {
                if (createPostViewModel) {
                    createPostViewModel.initialize()
                }
            }
            Component.onDestruction: {
                if (createPostViewModel) {
                    createPostViewModel.cleanup()
                }
            }
        }
    }

    Component {
        id: reviewCreateViewComponent
        ReviewCreateView {
            viewModel: createReviewViewModel

            onBackClicked: {
                createReviewViewModel.cleanup()
                stackView.pop()
            }

            onCreateSuccess: {
                createReviewViewModel.cleanup()
                stackView.pop()
                window.reviewCreated()
            }

            Component.onCompleted: {
                if (createReviewViewModel) {
                    createReviewViewModel.initialize()
                }
            }
            Component.onDestruction: {
                if (createReviewViewModel) {
                    createReviewViewModel.cleanup()
                }
            }
        }
    }

    Component {
        id: reviewUpdateViewComponent
        ReviewUpdateView {
            viewModel: updateReviewViewModel
            reviewId: typeof reviewId !== 'undefined' ? reviewId : 0
            reviewText: typeof reviewText !== 'undefined' ? reviewText : ""

            onBackClicked: {
                updateReviewViewModel.cleanup()
                stackView.pop()
            }

            onSaveCompleted: {
                updateReviewViewModel.cleanup()
                stackView.pop()
                window.reviewUpdated()
            }

            Component.onCompleted: {
                if (updateReviewViewModel) {
                    updateReviewViewModel.initialize()
                }
            }
            Component.onDestruction: {
                if (updateReviewViewModel) {
                    updateReviewViewModel.cleanup()
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
Component {
    id: postUpdateViewComponent
    PostUpdateView {
        viewModel: updatePostViewModel
        postId: typeof postId !== 'undefined' ? postId : 0
        postText: typeof postText !== 'undefined' ? postText : ""
        postCreatedAt: typeof postCreatedAt !== 'undefined' ? postCreatedAt : null
        
        onBackClicked: {
            updatePostViewModel.cleanup()
            stackView.pop()
        }
        
        onSaveCompleted: {
            updatePostViewModel.cleanup()
            stackView.pop()
            window.postUpdated()
        }
        
        Component.onCompleted: {
            if (updatePostViewModel) {
                updatePostViewModel.initialize()
            }
        }
        Component.onDestruction: {
            if (updatePostViewModel) {
                updatePostViewModel.cleanup()
            }
        }
    }
}

}
