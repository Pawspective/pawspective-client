import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    anchors.fill: parent

    property var searchOrganizationViewModel: null
    property var userViewModel: null
    
    signal profileRequested()
    signal organizationClicked(var organizationId)
    signal animalDetailRequested(int animalId)

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

    readonly property real leftPanelWidth: root.width * 0.7
    readonly property real rightPanelWidth: root.width * 0.3
    readonly property real contentMargins: root.height * 0.05
    readonly property real contentSpacing: root.height * 0.02

    property int currentTab: 1
    
    property alias searchInputText: searchInput.text
    
    TextField {
        id: searchInput
        visible: false  
    }

    color: theme.pageBg

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: root.leftPanelWidth
            color: theme.pageBg

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: root.contentMargins
                spacing: root.contentSpacing

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: root.height * 0.02

                    Text {
                        text: "Search"
                        font.family: theme.fontName
                        font.pixelSize: root.height * 0.045
                        font.bold: true
                        color: theme.textDark
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: root.width * 0.015

                        TabButton {
                            text: "Animals"
                            active: root.currentTab === 0
                            onClicked: root.currentTab = 0
                        }
                        TabButton {
                            text: "Organizations"
    active: root.currentTab === 1
    onClicked: {
        if (root.currentTab !== 1 && root.searchOrganizationViewModel) {
            root.searchOrganizationViewModel.initialize()  
        }
        root.currentTab = 1
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
                        id: contentLoader
                        anchors.fill: parent
                        anchors.margins: root.height * 0.02
                        sourceComponent: root.currentTab === 1 ? organizationsContent : animalsContent
                    }
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
                    active: true
                }
                SidebarItem {
                    text: "Organization"
                    onClicked: {
                        const rawOrganizationId = root.userViewModel ? root.userViewModel.userData.organizationId : null
                        root.organizationClicked(rawOrganizationId === undefined ? null : rawOrganizationId)
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

    function performSearch() {
        if (!root.searchOrganizationViewModel) {
            return
        }
        var query = ""
        if (contentLoader.item && contentLoader.item.searchInputField) {
            query = contentLoader.item.searchInputField.text.trim()
        }
        
        if (query.length === 0) {
            root.searchOrganizationViewModel.clearResults()
            return
        }
        
        if (query.length > 200) {
            query = query.substring(0, 200)
            if (contentLoader.item && contentLoader.item.searchInputField) {
                contentLoader.item.searchInputField.text = query
            }
        }
        root.searchOrganizationViewModel.searchQuery = query
        root.searchOrganizationViewModel.searchOrganizations()
    }
    
    function clearSearch() {
        if (root.searchOrganizationViewModel) {
            root.searchOrganizationViewModel.searchQuery = ""
            root.searchOrganizationViewModel.clearResults()
        }
        if (contentLoader.item && contentLoader.item.searchInputField) {
            contentLoader.item.searchInputField.text = ""
            contentLoader.item.searchInputField.forceActiveFocus()
        }
    }

    Component {
        id: organizationsContent
        Item {
            id: orgContentRoot
            
            property alias searchInputField: searchInputLocal
            
            ColumnLayout {
                anchors.fill: parent
                spacing: root.height * 0.015

                Rectangle {
                    Layout.fillWidth: true
                    implicitHeight: searchCardColumn.implicitHeight + root.height * 0.03
                    radius: 12
                    color: theme.pageBg
                    border.color: theme.purple
                    border.width: 1

                    ColumnLayout {
                        id: searchCardColumn
                        anchors.fill: parent
                        anchors.margins: root.height * 0.015
                        spacing: root.height * 0.015

                        Text {
                            text: "🔍 Find Organization"
                            font.family: theme.fontName
                            font.pixelSize: root.height * 0.028
                            font.bold: true
                            color: theme.textDark
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: root.height * 0.01

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: root.height * 0.06
                                radius: 10
                                color: theme.fieldBg
                                border.color: theme.accentPink
                                border.width: 1

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: root.height * 0.008
                                    spacing: root.height * 0.008

                                    TextField {
                                        id: searchInputLocal
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true
                                        font.family: theme.fontName
                                        font.pixelSize: root.height * 0.022
                                        color: theme.textDark
                                        placeholderText: "Enter organization name..."
                                        placeholderTextColor: theme.accentPink
                                        background: Rectangle {
                                            color: "transparent"
                                        }
                                        maximumLength: 200
                                        
                                        onAccepted: root.performSearch()
                                    }
                                }
                            }

                            CustomButton {
                                text: "Search"
                                baseColor: theme.purple
                                hoverColor: theme.accentPink
                                textColor: theme.buttonText
                                fontSize: root.height * 0.02
                                Layout.preferredWidth: root.width * 0.07
                                Layout.preferredHeight: root.height * 0.055
                                onClicked: root.performSearch()
                            }

                            CustomButton {
                                text: "Clear"
                                baseColor: theme.accentPink
                                hoverColor: theme.purple
                                textColor: theme.buttonText
                                fontSize: root.height * 0.02
                                Layout.preferredWidth: root.width * 0.07
                                Layout.preferredHeight: root.height * 0.055
                                visible: (root.searchOrganizationViewModel && 
                                         root.searchOrganizationViewModel.searchQuery.length > 0)
                                onClicked: root.clearSearch()
                            }
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    radius: 12
                    color: theme.fieldBg
                    border.color: theme.purple
                    border.width: 1

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: root.height * 0.015
                        spacing: root.height * 0.015

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: root.height * 0.045
                            radius: 8
                            color: theme.pageBg
                            visible: root.searchOrganizationViewModel && 
                                     (root.searchOrganizationViewModel.isSearching ||
                                      (root.searchOrganizationViewModel.searchQuery.length > 0 && 
                                       root.searchOrganizationViewModel.organizationsCount > 0))

                            Text {
                                anchors.centerIn: parent
                                text: {
                                    if (!root.searchOrganizationViewModel) return ""
                                    if (root.searchOrganizationViewModel.isSearching) {
                                        return "Searching..."
                                    }
                                    if (root.searchOrganizationViewModel.organizationsCount > 0) {
                                        return "Found: " + root.searchOrganizationViewModel.organizationsCount
                                    }
                                    return ""
                                }
                                font.family: theme.fontName
                                font.pixelSize: root.height * 0.018
                                color: theme.textDark
                                horizontalAlignment: Text.AlignHCenter
                                opacity: 0.8
                            }
                        }

                        ListView {
                            id: organizationsListView
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            clip: true
                            spacing: root.height * 0.012
                            
                            model: (root.searchOrganizationViewModel && 
                                   root.searchOrganizationViewModel.organizationsCount > 0) 
                                   ? root.searchOrganizationViewModel.organizations : []
                            
                            delegate: OrganizationCardView {
                                width: organizationsListView.width
                                organizationId: modelData.organizationId
                                organizationName: modelData.name
                                organizationCity: modelData.city
                                organizationDescription: modelData.description || ""
                                
                                onClicked: function(id) {
                                    root.organizationClicked(id)
                                }
                            }
                            
                            ScrollBar.vertical: ScrollBar {
                                policy: ScrollBar.AlwaysOn
                            }
                        }

                        Item {
                            anchors.centerIn: parent
                            width: root.height * 0.12
                            height: root.height * 0.12
                            visible: root.searchOrganizationViewModel && root.searchOrganizationViewModel.isSearching

                            Image {
                                id: pawIcon
                                anchors.centerIn: parent
                                width: parent.width
                                height: parent.height
                                source: "../resources/paw.png"
                                fillMode: Image.PreserveAspectFit
                                smooth: true
                                
                                RotationAnimation on rotation {
                                    id: pawRotation
                                    from: 0
                                    to: 360
                                    duration: 1500
                                    loops: Animation.Infinite
                                    running: parent.parent.visible
                                }
                            }
                            
                            Text {
                                anchors.centerIn: parent
                                text: "🐾"
                                font.pixelSize: root.height * 0.08
                                visible: false
                            }
                        }

                        ColumnLayout {
                            anchors.centerIn: parent
                            spacing: root.height * 0.015
                            visible: organizationsListView.count === 0 && 
                                     root.searchOrganizationViewModel && 
                                     root.searchOrganizationViewModel.searchQuery.length > 0 && 
                                     !root.searchOrganizationViewModel.isSearching

                                Image {
                                    source: "../resources/sad_cat.png"
                                    fillMode: Image.PreserveAspectFit
                                    smooth: true
                                    Layout.alignment: Qt.AlignHCenter
                                    Layout.preferredWidth: root.width * 0.25
                                    Layout.preferredHeight: root.height * 0.25
                                }

                                Text {
                                    text: "Nothing found\nfor \"" + (root.searchOrganizationViewModel ? root.searchOrganizationViewModel.searchQuery : "") + "\""
                                    font.family: theme.fontName
                                    font.pixelSize: root.height * 0.024
                                    font.bold: true
                                    color: theme.textDark
                                    horizontalAlignment: Text.AlignHCenter
                                    opacity: 0.8
                                    Layout.alignment: Qt.AlignHCenter
                                }
                                
                            }
                    }
                }
            }
        }
    }

    Component {
        id: animalsContent

        ColumnLayout {
            anchors.fill: parent
            spacing: root.height * 0.015

            Item { Layout.fillHeight: true }

            ColumnLayout {
                Layout.alignment: Qt.AlignCenter
                spacing: root.height * 0.025

                Image {
                    source: "../resources/sad_cat.png"
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                    Layout.alignment: Qt.AlignHCenter
                    Layout.preferredWidth: root.width * 0.35
                    Layout.preferredHeight: root.height * 0.35
                }

            }

            Item { Layout.fillHeight: true }
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
        Layout.preferredHeight: root.height * 0.07
        radius: 12
        color: active ? theme.purple : (hovered ? theme.accentPink : theme.pageBg)
        border.color: theme.purple
        border.width: 1

        Text {
            anchors.centerIn: parent
            text: parent.text
            font.family: theme.fontName
            font.pixelSize: root.height * 0.024
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