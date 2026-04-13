import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts

Rectangle {
    id: root
    anchors.fill: parent

    // Optional context for reusable organization screen.
    // organizationId === null means "create organization" mode.
    property var organizationId: null

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
            } else {
                organizationViewModel.setCanUpdateOrganization(false)
                organizationViewModel.initializeForCreateOrganization()
            }
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
                sourceComponent: root.hasOrganization ? organizationProfileComponent : createOrganizationComponent
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
                    onClicked: root.profileRequested()
                }
                SidebarItem {
                    text: "Search"
                    onClicked: root.searchRequested()
                }
                SidebarItem {
                    text: "Organization"
                    active: true
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
            
            function reloadAnimals() {
                if (animalListViewModel && organizationViewModel) {
                    var orgId = organizationViewModel.currentOrganizationId
                    if (orgId > 0 && orgId !== lastLoadedOrgId) {
                        lastLoadedOrgId = orgId
                        animalListViewModel.loadAnimalsForOrganization(orgId)
                    }
                }
            }
            
            // TODO: This double initalize 
            Component.onCompleted: {
                if (animalListViewModel && !initialized) {
                    animalListViewModel.initialize()
                    initialized = true
                }
                
                reloadAnimals()
                
                if (organizationViewModel) {
                    organizationViewModel.currentOrganizationIdChanged.connect(reloadAnimals)
                }
                
                if (createAnimalViewModel) {
                    createAnimalViewModel.creationFinished.connect(function(success) {
                        if (success) {
                            reloadAnimals()
                        }
                    })
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
        
            // Animal list view
            AnimalListView {
                headerComponent: canUpdateOrganization ? createButtonComponent : null
                Layout.fillWidth: true
                Layout.fillHeight: true
                viewModel: animalListViewModel
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
