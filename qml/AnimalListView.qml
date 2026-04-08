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

        header: headerComponent ? headerWrapper : null
        
        Component {
            id: headerWrapper
            Item {
                width: animalListView.width
                height: header.height + headerMargin
                
                Loader {
                    id: header
                    sourceComponent: headerComponent
                    width: parent.width
                }
                
                property real headerMargin: root.height * 0.012
            }
        }

        spacing: root.height * 0.012
        delegate: AnimalCardView {
            width: animalListView.width
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
            policy: ScrollBar.AsNeeded 
        }

        model: root.viewModel ? root.viewModel.listModel : null

        Component.onDestruction: {
            if (root.viewModel) {
                root.viewModel.cleanup()
            }
        }
    }
    
    Text {
        anchors.centerIn: parent
        visible: animalListView.count === 0
        text: "No animals"
        font.family: root.fontName
        font.pixelSize: root.height * 0.05
        color: root.textDark
    }
}
