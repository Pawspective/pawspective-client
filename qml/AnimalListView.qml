import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    width: parent ? parent.width : implicitWidth
    height: parent ? parent.height : implicitHeight

    // If viewModel is not passed, use global animalListViewModel
    property var viewModel: typeof animalListViewModel !== 'undefined' ? animalListViewModel : null
    property Component headerComponent: null
    property bool showEmptyImage: false  // Show sad_cat image when list is empty
    property bool showPaginationControls: true

    readonly property color textDark: "#8572af"
    readonly property string fontName: "Comic Sans MS"

    readonly property bool hasPagination: viewModel && viewModel.totalPages > 1 && showPaginationControls

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

        footer: (root.viewModel && (root.viewModel.isLoading || (typeof root.viewModel.listModel !== 'undefined' && root.viewModel.listModel.count === 0)))
            ? emptyOrLoadingFooter : null
        
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

    // Builds the list of page buttons to display:
    // [1] [...] [cur-1] [cur] [cur+1] [...] [last]
    function buildPageWindow() {
        if (!viewModel || viewModel.totalPages <= 1) return []
        const cur = viewModel.currentPage
        const last = viewModel.totalPages
        const window = 2  // pages on each side of current
        let pages = []

        // always include page 1
        pages.push(1)

        const winStart = Math.max(2, cur - window)
        const winEnd = Math.min(last - 1, cur + window)

        if (winStart > 2) pages.push(-1)  // left ellipsis

        for (let p = winStart; p <= winEnd; p++) pages.push(p)

        if (winEnd < last - 1) pages.push(-1)  // right ellipsis

        // always include last page (if more than 1 page total)
        if (last > 1) pages.push(last)

        return pages
    }

    Row {
        id: paginationRow
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: root.height * 0.01
        spacing: root.width * 0.01
        visible: root.hasPagination

        // ← Prev
        CustomButton {
            id: prevButton
            text: "<"
            enabled: root.viewModel && root.viewModel.currentPage > 1 && !root.viewModel.isLoading
            onClicked: root.viewModel.prevPage()
            implicitWidth: Math.min(root.width, root.height) * 0.09
            implicitHeight: Math.min(root.width, root.height) * 0.07
            fontSize: Math.min(root.width, root.height) * 0.035
            baseColor: prevButton.enabled ? "#8572af" : "#c4b8e0"
            hoverColor: "#7060a0"
            clickColor: "#5a4a8a"
            textColor: "white"
            radius: 6
        }

        // Page number buttons
        Repeater {
            model: root.buildPageWindow()

            delegate: Item {
                implicitWidth: modelData === -1
                    ? Math.min(root.width, root.height) * 0.05
                    : Math.min(root.width, root.height) * 0.09
                implicitHeight: Math.min(root.width, root.height) * 0.07
                anchors.verticalCenter: parent.verticalCenter

                // Ellipsis
                Text {
                    anchors.centerIn: parent
                    visible: modelData === -1
                    text: "..."
                    font.family: root.fontName
                    font.pixelSize: Math.min(root.width, root.height) * 0.035
                    color: root.textDark
                }

                // Page button
                CustomButton {
                    id: pageBtn
                    anchors.fill: parent
                    visible: modelData !== -1
                    enabled: !root.viewModel.isLoading && modelData !== root.viewModel.currentPage
                    onClicked: root.viewModel.goToPage(modelData)

                    readonly property bool isCurrent: modelData === (root.viewModel ? root.viewModel.currentPage : -1)

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

        // Next →
        CustomButton {
            id: nextButton
            text: ">"
            enabled: root.viewModel && root.viewModel.currentPage < root.viewModel.totalPages && !root.viewModel.isLoading
            onClicked: root.viewModel.nextPage()
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
