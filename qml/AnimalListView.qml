import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    anchors.fill: parent

    // If viewModel is not passed, use global animalListViewModel
    property var viewModel: typeof animalListViewModel !== 'undefined' ? animalListViewModel : null
    property Component headerComponent: null
    property bool showEmptyImage: false  // Show sad_cat image when list is empty

    readonly property color textDark: "#8572af"
    readonly property string fontName: "Comic Sans MS"

    readonly property bool hasPagination: viewModel && viewModel.totalPages > 1

    ListView {
        id: animalListView
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: hasPagination ? paginationRow.top : parent.bottom
        // anchors.margins: root.height * 0.015

        clip: true
        boundsBehavior: Flickable.StopAtBounds
        
        readonly property real scrollBarMargin: verticalScrollBar.width + 6

        header: headerComponent ? headerWrapper : null
        
        Component {
            id: headerWrapper
            Item {
                width: animalListView.width - animalListView.scrollBarMargin
                height: header.height + headerMargin
                
                Loader {
                    id: header
                    sourceComponent: headerComponent
                    width: parent.width
                }
                
                property real headerMargin: root.height * 0.012
            }
        }

        footer: (animalListView.count === 0 || root.viewModel?.isLoading) ? emptyOrLoadingFooter : null
        
        Component {
            id: emptyOrLoadingFooter
            Item {
                width: animalListView.width - animalListView.scrollBarMargin
                implicitHeight: Math.max(columnContent.implicitHeight + Math.min(root.width, root.height) * 0.04, Math.min(root.width, root.height) * 0.2)
                
                LoaderSpinner {
                    anchors.centerIn: parent
                    width: parent.width * 0.2
                    height: width
                    running: root.viewModel ? root.viewModel.isLoading : false
                    visible: running
                }
                
                ColumnLayout {
                    id: columnContent
                    anchors.centerIn: parent
                    spacing: Math.min(root.width, root.height) * 0.005
                    visible: !root.viewModel?.isLoading
                    
                    Image {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredWidth: Math.min(root.width, root.height) * 0.6
                        Layout.preferredHeight: Math.min(root.width, root.height) * 0.6
                        Layout.bottomMargin: -Math.min(root.width, root.height) * 0.12
                        source: "../resources/sad_cat.png"
                        fillMode: Image.PreserveAspectFit
                        smooth: true
                        visible: root.showEmptyImage
                    }
                
                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.topMargin: Math.min(root.width, root.height) * 0.01
                        text: "No animals"
                        font.family: root.fontName
                        font.pixelSize: Math.min(root.width, root.height) * 0.04
                        color: root.textDark
                    }
                }
            }
        }

        spacing: root.height * 0.012
        delegate: AnimalCardView {
            width: animalListView.width - animalListView.scrollBarMargin
            animalName: model.animalName ? model.animalName : ""
            animalDescription: model.animalDescription ? model.animalDescription : ""
            animalAge: model.animalAge ? model.animalAge : 0
            animalType: model.animalType ? model.animalType : ""
            animalId: model.animalId ? model.animalId : -1
            onClicked: function(animalId) {
                stackView.push(animalDetailViewComponent, { animalId: animalId })
            }
        }

        ScrollBar.vertical: ScrollBar { 
            id: verticalScrollBar
            policy: ScrollBar.AsNeeded 
        }

        model: root.viewModel ? root.viewModel.listModel : null

        Component.onDestruction: {
            if (root.viewModel) {
                root.viewModel.cleanup()
            }
        }
    }

    Row {
        id: paginationRow
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: root.height * 0.01
        spacing: root.width * 0.02
        visible: root.hasPagination

        Button {
            id: prevButton
            text: "<"
            enabled: root.viewModel && root.viewModel.currentPage > 1 && !root.viewModel.isLoading
            onClicked: root.viewModel.prevPage()
            background: Rectangle {
                color: prevButton.enabled ? "#8572af" : "#c4b8e0"
                radius: 6
            }
            contentItem: Text {
                text: prevButton.text
                font.family: root.fontName
                font.pixelSize: Math.min(root.width, root.height) * 0.04
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: root.viewModel ? (root.viewModel.currentPage + " / " + root.viewModel.totalPages) : ""
            font.family: root.fontName
            font.pixelSize: Math.min(root.width, root.height) * 0.035
            color: root.textDark
        }

        Button {
            id: nextButton
            text: ">"
            enabled: root.viewModel && root.viewModel.currentPage < root.viewModel.totalPages && !root.viewModel.isLoading
            onClicked: root.viewModel.nextPage()
            background: Rectangle {
                color: nextButton.enabled ? "#8572af" : "#c4b8e0"
                radius: 6
            }
            contentItem: Text {
                text: nextButton.text
                font.family: root.fontName
                font.pixelSize: Math.min(root.width, root.height) * 0.04
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
