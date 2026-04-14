import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    anchors.fill: parent
    
    // If viewModel is not passed, use global animalListViewModel
    property var viewModel: typeof animalListViewModel !== 'undefined' ? animalListViewModel : null
    property Component headerComponent: null
    
    readonly property color textDark: "#8572af"
    readonly property string fontName: "Comic Sans MS"



    ListView {
        id: animalListView
        anchors.fill: parent
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
                height: Math.max(animalListView.height * 0.75, root.height * 0.3)
                
                LoaderSpinner {
                    anchors.centerIn: parent
                    width: parent.width * 0.2
                    height: width
                    running: root.viewModel ? root.viewModel.isLoading : false
                    visible: running
                }
                
                Text {
                    anchors.centerIn: parent
                    text: "No animals"
                    font.family: root.fontName
                    font.pixelSize: root.height * 0.05
                    color: root.textDark
                    visible: !root.viewModel?.isLoading
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
}
