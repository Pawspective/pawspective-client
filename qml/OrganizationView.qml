import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts

Rectangle {
    id: root

    // Optional context for reusable organization screen.
    // organizationId === null means "create organization" mode.
    property var organizationId: null
    property string navigationSource: "sidebar"  // Track how we got here: "sidebar", "search", "animal"

    QtObject {
        id: theme
        readonly property string fontName: "Comic Sans MS"
        readonly property color pageBg: "#e8d8cb"
        readonly property color sidebarBg: "#e9bebb"
        readonly property color purple: "#b8abd7"
        readonly property color fieldBg: "#fdfdfd"
        readonly property color accentPink: "#f4a7b9"
        readonly property color textDark: "#8572af"
        readonly property color buttonText: "#e7ebf5"
    }

    readonly property bool canUpdateOrganization: organizationViewModel ? organizationViewModel.canUpdateOrganization : false
    readonly property bool hasOrganization: organizationViewModel ? organizationViewModel.hasOrganization : false
    readonly property string organizationName: organizationViewModel ? organizationViewModel.organizationName : ""
    readonly property string organizationCity: organizationViewModel ? organizationViewModel.organizationCity : ""
    readonly property string organizationDescription: organizationViewModel ? organizationViewModel.organizationDescription : ""

    property bool showDescription: organizationViewModel ? organizationViewModel.showDescription : false
    property int currentTab: organizationViewModel ? organizationViewModel.currentTab : 1

    signal profileRequested()
    signal searchRequested()
    signal organizationRequested(int orgId)
    signal createOrganizationClicked()
    signal updateOrganizationClicked()
    signal createAnimalRequested()
    signal animalDetailRequested(int animalId)
    signal backClicked()

    readonly property real leftPanelWidth: root.width * 0.7
    readonly property real rightPanelWidth: root.width * 0.3
    readonly property real contentMargins: root.height * 0.05
    readonly property real contentSpacing: root.height * 0.02

    color: theme.pageBg

    Component.onCompleted: {
        if (organizationViewModel) {
            const requestedOrganizationId = Number(root.organizationId)
            const currentUserOrganizationId = Number(userViewModel ? userViewModel.userData.organizationId : null)
            const canUpdateFromRequest = Number.isFinite(requestedOrganizationId)
                                         && requestedOrganizationId > 0
                                         && requestedOrganizationId === currentUserOrganizationId

            if (root.organizationId !== null) {
                organizationViewModel.setCanUpdateOrganization(canUpdateFromRequest)
                organizationViewModel.initializeForOrganization(root.organizationId)
            } else if (currentUserOrganizationId > 0) {
                organizationViewModel.setCanUpdateOrganization(true)
                organizationViewModel.initializeForOrganization(currentUserOrganizationId)
            } else if (userViewModel && userViewModel.isBusy) {
                return
            } else {
                organizationViewModel.setCanUpdateOrganization(false)
                organizationViewModel.initializeForCreateOrganization()
            }
        }
    }

    Connections {
        target: userViewModel
        function onUserDataLoaded() {
            if (!organizationViewModel || organizationViewModel.isBusy) {
                return
            }

            if (root.organizationId !== null) {
                return
            }

            const currentUserOrganizationId = Number(userViewModel.userData.organizationId)
            if (currentUserOrganizationId > 0) {
                if (organizationViewModel.hasOrganization
                    && organizationViewModel.currentOrganizationId === currentUserOrganizationId) {
                    return
                }
                organizationViewModel.setCanUpdateOrganization(true)
                organizationViewModel.initializeForOrganization(currentUserOrganizationId)
            } else {
                organizationViewModel.setCanUpdateOrganization(false)
                organizationViewModel.initializeForCreateOrganization()
            }
        }
        function onUserDataLoadFailed() {
            if (!organizationViewModel || organizationViewModel.isBusy) {
                return
            }

            if (root.organizationId !== null) {
                return
            }

            organizationViewModel.setCanUpdateOrganization(false)
            organizationViewModel.initializeForCreateOrganization()
        }
    }

    // Sidebar
    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: root.leftPanelWidth
            color: theme.pageBg

            Loader {
                anchors.fill: parent
                anchors.margins: root.contentMargins
                sourceComponent: (organizationViewModel && organizationViewModel.isBusy)
                                 || (userViewModel
                                     && userViewModel.isBusy
                                     && root.organizationId === null
                                     && Number(userViewModel.userData.organizationId || 0) <= 0)
                                 ? emptyOrganizationComponent
                                 : (root.hasOrganization
                                    ? organizationProfileComponent
                                    : createOrganizationComponent)
            }

            // Back Button - positioned absolutely, doesn't affect layout
            Rectangle {
                id: backButton
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.topMargin: root.height * 0.005
                anchors.leftMargin: root.contentMargins
                width: root.height * 0.15
                height: root.height * 0.04
                color: backArea.containsMouse ? theme.accentPink : theme.purple
                // TODO: This should be change when we change stack logic
                visible: stackView.depth > 1 && root.hasOrganization && !root.canUpdateOrganization
                z: 10

                Text {
                    anchors.centerIn: parent
                    text: "←"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: 18
                    anchors.verticalCenterOffset: -3
                    anchors.horizontalCenterOffset: 0
                    scale: 1.5
                    color: theme.buttonText
                }

                MouseArea {
                    id: backArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.backClicked()
                }
            }
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: root.rightPanelWidth
            color: theme.sidebarBg

            ColumnLayout {
                anchors.fill: parent
                anchors.topMargin: root.height * 0.08
                spacing: root.height * 0.02

                SidebarItem {
                    text: "Profile"
                    active: root.navigationSource === "profile"
                    onClicked: {
                        if (!active) {
                            root.profileRequested()
                        }
                    }
                }
                SidebarItem {
                    text: "Search"
                    active: root.navigationSource === "search"
                    onClicked: {
                        if (!active) {
                            root.searchRequested()
                        }
                    }
                }
                SidebarItem {
                    text: "Organization"
                    active: root.navigationSource === "sidebar" 
                    onClicked: {
                        if (!active) {
                            const rawOrganizationId = userViewModel ? userViewModel.userData.organizationId : null
                            root.organizationRequested(rawOrganizationId === undefined ? null : rawOrganizationId)
                        }
                    }
                }

                Image {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.preferredWidth: root.width * 0.25
                    Layout.preferredHeight: root.height * 0.7
                    Layout.topMargin: -root.height * 0.05
                    source: "../resources/lilac.png"
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                }

                Item { Layout.fillHeight: true }
            }
        }
    }

    Component {
        id: createOrganizationComponent

        ColumnLayout {
            anchors.fill: parent
            spacing: root.contentSpacing

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 16
                color: theme.fieldBg

                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: root.height * 0.03

                    Image {
                        source: "../resources/sad_cat.png"
                        fillMode: Image.PreserveAspectFit
                        smooth: true
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: root.width * 0.42
                        Layout.preferredHeight: root.height * 0.42
                    }

                    Text {
                        text: "You do not have an organization profile yet"
                        font.family: theme.fontName
                        font.pixelSize: root.height * 0.03
                        color: theme.textDark
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignHCenter
                        horizontalAlignment: Text.AlignHCenter
                    }

                    CustomButton {
                        text: "Create Organization"
                        baseColor: theme.purple
                        hoverColor: theme.accentPink
                        textColor: theme.buttonText
                        fontSize: Math.min(root.height * 0.035, root.width * 0.018)
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: root.width * 0.24
                        Layout.preferredHeight: root.height * 0.08
                        onClicked: {
                            if (organizationViewModel) {
                                organizationViewModel.createOrganization()
                            }
                            root.createOrganizationClicked()
                        }
                    }
                }
            }
        }
    }

    Component {
        id: emptyOrganizationComponent

        Item { }
    }

    Component {
        id: organizationProfileComponent

        ColumnLayout {
            anchors.fill: parent
            spacing: root.contentSpacing

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: headerColumn.implicitHeight + root.height * 0.04
                radius: 16
                color: theme.fieldBg
                border.color: theme.purple
                border.width: 1

                ColumnLayout {
                    id: headerColumn
                    anchors.fill: parent
                    anchors.margins: root.height * 0.02
                    spacing: root.height * 0.015

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: root.width * 0.02

                        Rectangle {
                            Layout.preferredWidth: root.height * 0.12
                            Layout.preferredHeight: root.height * 0.12
                            radius: width / 2
                            color: theme.pageBg
                            border.color: theme.purple
                            border.width: 1

                            Text {
                                anchors.centerIn: parent
                                text: "Avatar"
                                font.family: theme.fontName
                                font.pixelSize: root.height * 0.02
                                color: theme.textDark
                            }
                        }

                        ColumnLayout {
                            spacing: root.height * 0.005

                            Text {
                                text: root.organizationName
                                font.family: theme.fontName
                                font.pixelSize: root.height * 0.04
                                font.bold: true
                                color: theme.textDark
                            }

                            Text {
                                text: root.organizationCity
                                font.family: theme.fontName
                                font.pixelSize: root.height * 0.025
                                color: theme.accentPink
                            }
                        }

                        Item { Layout.fillWidth: true }

                        ColumnLayout {
                            spacing: root.height * 0.01

                            CustomButton {
                                text: root.showDescription ? "Hide Description" : "Show Description"
                                baseColor: theme.purple
                                hoverColor: theme.accentPink
                                textColor: theme.buttonText
                                fontSize: Math.min(root.height * 0.027, root.width * 0.015)
                                Layout.preferredWidth: root.width * 0.18
                                Layout.preferredHeight: root.height * 0.07
                                onClicked: {
                                    if (organizationViewModel) {
                                        organizationViewModel.showDescription = !organizationViewModel.showDescription
                                    }
                                }
                            }

                            CustomButton {
                                visible: root.canUpdateOrganization
                                text: "Update Organization Data"
                                baseColor: theme.purple
                                hoverColor: theme.accentPink
                                textColor: theme.buttonText
                                fontSize: Math.min(root.height * 0.024, root.width * 0.013)
                                Layout.preferredWidth: root.width * 0.18
                                Layout.preferredHeight: root.height * 0.07
                                onClicked: {
                                    if (organizationViewModel) {
                                        organizationViewModel.updateOrganization()
                                    }
                                    root.updateOrganizationClicked()
                                }
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        visible: root.showDescription
                        implicitHeight: root.height * 0.16
                        radius: 12
                        color: theme.pageBg
                        border.color: theme.accentPink
                        border.width: 1

                        Text {
                            anchors.fill: parent
                            anchors.margins: root.height * 0.015
                            text: root.organizationDescription
                            wrapMode: Text.WordWrap
                            font.family: theme.fontName
                            font.pixelSize: root.height * 0.024
                            color: theme.textDark
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: root.width * 0.015

                TabButton {
                    text: "Reviews"
                    active: root.currentTab === 0
                    onClicked: {
                        if (organizationViewModel) {
                            organizationViewModel.currentTab = 0
                        }
                    }
                }
                TabButton {
                    text: "Animals"
                    active: root.currentTab === 1
                    onClicked: {
                        if (organizationViewModel) {
                            organizationViewModel.currentTab = 1
                        }
                    }
                }
                TabButton {
                    text: "Posts"
                    active: root.currentTab === 2
                    onClicked: {
                        if (organizationViewModel) {
                            organizationViewModel.currentTab = 2
                        }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 16
                color: theme.fieldBg

                Loader {
                    anchors.fill: parent
                    anchors.margins: root.height * 0.02
                    sourceComponent: root.currentTab === 0
                                     ? reviewsContent
                                     : (root.currentTab === 1 ? animalsContent : postsContent)
                }
            }
        }
    }

    // Review tab
    Component {
        id: reviewsContent

        Text {
            text: "Reviews section placeholder"
            font.family: theme.fontName
            font.pixelSize: root.height * 0.03
            color: theme.textDark
        }
    }

    // Animal tab
    Component {
        id: animalsContent
        Item {
            anchors.fill: parent
            
            property bool initialized: false
            property var lastLoadedOrgId: -1
            readonly property bool hasPagination: animalListViewModel && animalListViewModel.totalPages > 1
            
            function reloadAnimals() {
                if (animalListViewModel && organizationViewModel) {
                    var orgId = organizationViewModel.currentOrganizationId
                    if (orgId > 0 && orgId !== lastLoadedOrgId) {
                        lastLoadedOrgId = orgId
                        animalListViewModel.loadAnimalsForOrganization(orgId)
                    }
                }
            }

            function buildPageWindow() {
                if (!animalListViewModel || animalListViewModel.totalPages <= 1) return []
                const cur = animalListViewModel.currentPage
                const last = animalListViewModel.totalPages
                const window = 2
                let pages = []

                pages.push(1)

                const winStart = Math.max(2, cur - window)
                const winEnd = Math.min(last - 1, cur + window)

                if (winStart > 2) pages.push(-1)

                for (let p = winStart; p <= winEnd; p++) pages.push(p)

                if (winEnd < last - 1) pages.push(-1)

                if (last > 1) pages.push(last)

                return pages
            }
            
            Component.onCompleted: {
                if (animalListViewModel && !initialized) {
                    animalListViewModel.initialize()
                    initialized = true
                }
                
                reloadAnimals()
                
                if (organizationViewModel) {
                    organizationViewModel.currentOrganizationIdChanged.connect(reloadAnimals)
                }
            }
            
            Component.onDestruction: {
                if (animalListViewModel) {
                    animalListViewModel.cleanup()
                    initialized = false
                }
            }

            Component {
                id: createButtonComponent
                CustomButton {
                    text: "+ Create Animal"
                    baseColor: theme.purple
                    hoverColor: theme.accentPink
                    textColor: theme.buttonText
                    fontSize: root.height * 0.025
                    Layout.alignment: Qt.AlignRight
                    Layout.preferredWidth: root.width * 0.15
                    Layout.preferredHeight: root.height * 0.06
                    Layout.rightMargin: root.height * 0.02
                    Layout.topMargin: root.height * 0.02
                    onClicked: {
                        if (createAnimalViewModel && organizationViewModel) {
                            var orgId = organizationViewModel.currentOrganizationId
                            createAnimalViewModel.setOrganizationId(orgId)
                        }
                        root.createAnimalRequested()
                    }
                }
            }
        
            ColumnLayout {
                anchors.fill: parent
                spacing: root.height * 0.01

                Item {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    // Animal list view
                    AnimalListView {
                        id: organizationAnimalList
                        anchors.fill: parent
                        headerComponent: canUpdateOrganization ? createButtonComponent : null
                        viewModel: animalListViewModel
                        showPaginationControls: false
                    }
                }

                Row {
                    id: paginationRow
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: root.height * 0.01
                    spacing: root.width * 0.01
                    visible: hasPagination

                    CustomButton {
                        id: prevButton
                        text: "<"
                        enabled: animalListViewModel && animalListViewModel.currentPage > 1 && !animalListViewModel.isLoading
                        onClicked: animalListViewModel.prevPage()
                        implicitWidth: Math.min(root.width, root.height) * 0.09
                        implicitHeight: Math.min(root.width, root.height) * 0.07
                        fontSize: Math.min(root.width, root.height) * 0.035
                        baseColor: prevButton.enabled ? "#8572af" : "#c4b8e0"
                        hoverColor: "#7060a0"
                        clickColor: "#5a4a8a"
                        textColor: "white"
                        radius: 6
                    }

                    Repeater {
                        model: buildPageWindow()

                        delegate: Item {
                            implicitWidth: modelData === -1
                                ? Math.min(root.width, root.height) * 0.05
                                : Math.min(root.width, root.height) * 0.09
                            implicitHeight: Math.min(root.width, root.height) * 0.07
                            anchors.verticalCenter: parent.verticalCenter

                            Text {
                                anchors.centerIn: parent
                                visible: modelData === -1
                                text: "..."
                                font.family: theme.fontName
                                font.pixelSize: Math.min(root.width, root.height) * 0.035
                                color: theme.textDark
                            }

                            CustomButton {
                                id: pageBtn
                                anchors.fill: parent
                                visible: modelData !== -1
                                enabled: !animalListViewModel.isLoading && modelData !== animalListViewModel.currentPage
                                onClicked: animalListViewModel.goToPage(modelData)

                                readonly property bool isCurrent: modelData === (animalListViewModel ? animalListViewModel.currentPage : -1)

                                text: modelData !== -1 ? String(modelData) : ""
                                fontSize: Math.min(root.width, root.height) * 0.032
                                baseColor: "#00f0ecf9"
                                hoverColor: "#f0ecf9"
                                clickColor: "#e0d8f5"
                                textColor: pageBtn.isCurrent ? "#5a4a8a"
                                         : pageBtn.enabled   ? "#8572af"
                                         :                    "#c4b8e0"
                                border.color: pageBtn.isCurrent ? "#5a4a8a" : "transparent"
                                border.width: pageBtn.isCurrent ? 2 : 0
                                radius: 6
                            }
                        }
                    }

                    CustomButton {
                        id: nextButton
                        text: ">"
                        enabled: animalListViewModel && animalListViewModel.currentPage < animalListViewModel.totalPages
                                 && !animalListViewModel.isLoading
                        onClicked: animalListViewModel.nextPage()
                        implicitWidth: Math.min(root.width, root.height) * 0.09
                        implicitHeight: Math.min(root.width, root.height) * 0.07
                        fontSize: Math.min(root.width, root.height) * 0.035
                        baseColor: nextButton.enabled ? "#8572af" : "#c4b8e0"
                        hoverColor: "#7060a0"
                        clickColor: "#5a4a8a"
                        textColor: "white"
                        radius: 6
                    }
                }
            }
        }
    }

    // Post tab
    Component {
        id: postsContent

        Text {
            text: "Posts section placeholder"
            font.family: theme.fontName
            font.pixelSize: root.height * 0.03
            color: theme.textDark
        }
    }

    component SidebarItem : Rectangle {
        property string text: ""
        property bool active: false
        property bool hovered: false
        signal clicked()

        Layout.fillWidth: true
        height: root.height * 0.07
        radius: 8
        color: active ? theme.purple : (hovered ? theme.accentPink : "transparent")

        Text {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: root.width * 0.02
            text: parent.text
            font.family: theme.fontName
            font.pixelSize: root.height * 0.025
            font.bold: parent.active
            color: parent.active ? "white" : theme.textDark
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: parent.hovered = true
            onExited: parent.hovered = false
            onClicked: parent.clicked()
        }
    }

    component TabButton : Rectangle {
        property string text: ""
        property bool active: false
        property bool hovered: false
        signal clicked()

        Layout.fillWidth: true
        Layout.preferredHeight: root.height * 0.075
        radius: 12
        color: active ? theme.purple : (hovered ? theme.accentPink : theme.pageBg)
        border.color: theme.purple
        border.width: 1

        Text {
            anchors.centerIn: parent
            text: parent.text
            font.family: theme.fontName
            font.pixelSize: root.height * 0.026
            font.bold: parent.active
            color: parent.active ? theme.buttonText : theme.textDark
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: parent.hovered = true
            onExited: parent.hovered = false
            onClicked: parent.clicked()
        }
    }
}
