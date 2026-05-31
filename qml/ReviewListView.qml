import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    width: parent ? parent.width : implicitWidth
    height: parent ? parent.height : implicitHeight

    property var viewModel: typeof reviewListViewModel !== 'undefined' ? reviewListViewModel : null
    property var model: null
    property Component headerComponent: null
    property bool showPaginationControls: true
    property real paginationScale: Math.min(root.width, root.height) * 1.75

    signal editRequested(int reviewId)
    signal animalRequested(int animalId)

    readonly property color textDark: "#8572af"
    readonly property string fontName: "Comic Sans MS"

    readonly property bool hasPagination: viewModel && viewModel.totalPages > 1 && showPaginationControls

    readonly property bool isLoading: root.viewModel ? root.viewModel.isLoading : false
    readonly property int modelCount: {
        if (root.model && typeof root.model.count !== "undefined") {
            return root.model.count
        }
        if (root.viewModel && root.viewModel.listModel && typeof root.viewModel.listModel.count !== "undefined") {
            return root.viewModel.listModel.count
        }
        return -1
    }

    ListView {
        id: reviewListView
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: hasPagination ? paginationRow.top : parent.bottom
        anchors.bottomMargin: hasPagination ? root.paginationScale * 0.01 : 0

        clip: true
        boundsBehavior: Flickable.StopAtBounds

        readonly property real scrollBarMargin: verticalScrollBar.width + 6

        header: headerComponent ? headerWrapper : null

        Component {
            id: headerWrapper
            Item {
                width: reviewListView.width - reviewListView.scrollBarMargin
                height: header.height + headerMargin

                Loader {
                    id: header
                    sourceComponent: headerComponent
                    width: parent.width
                }

                property real headerMargin: root.height * 0.012
            }
        }

        footer: root.isLoading ? loadingFooter : null

        Component {
            id: loadingFooter
            Item {
                width: reviewListView.width - reviewListView.scrollBarMargin
                implicitHeight: Math.min(root.width, root.height) * 0.2

                LoaderSpinner {
                    anchors.centerIn: parent
                    width: parent.width * 0.2
                    height: width
                    running: root.isLoading
                    visible: running
                }
            }
        }

        spacing: root.height * 0.012
        delegate: ReviewCardView {
            width: reviewListView.width - reviewListView.scrollBarMargin
            reviewId: model.id ? model.id : -1
            animalId: model.animalId ? model.animalId : -1
            animalName: model.animalName ? model.animalName : ""
            reviewText: model.text ? model.text : ""
            reviewCreatedAt: model.createdAt ? model.createdAt : null
            canEdit: model.canEdit !== undefined
                ? model.canEdit
                : (model.can_edit !== undefined ? model.can_edit : false)
            onEditRequested: function(reviewId) { root.editRequested(reviewId) }
            onDeleteRequested: function(reviewId) { deleteConfirmDialog.reviewId = reviewId; deleteConfirmDialog.open() }
            onAnimalClicked: function(animalId) { root.animalRequested(animalId) }
        }

        ScrollBar.vertical: ScrollBar {
            id: verticalScrollBar
            policy: ScrollBar.AsNeeded
        }

        model: root.model ? root.model : (root.viewModel ? root.viewModel.listModel : null)
    }

    Dialog {
        id: deleteConfirmDialog
        property int reviewId: -1
        modal: true
        parent: Window.window ? Window.window.overlay : Overlay.overlay
        
        // Задаем ширину (80% от ширины всего окна приложения)
        width: Window.window ? Window.window.width * 0.8 : 300
        
        // Математическое выравнивание строго по центру родителя (оверлея окна)
        x: parent ? (parent.width - width) / 2 : 0
        y: parent ? (parent.height - height) / 2 : 0
        title: "Delete Review"
        standardButtons: Dialog.Yes | Dialog.No

        onAccepted: {
            if (root.viewModel && deleteConfirmDialog.reviewId > 0) {
                var id = deleteConfirmDialog.reviewId
                root.viewModel.deleteReview(id)
                deleteConfirmDialog.reviewId = -1
            }
        }

        contentItem: Text {
            text: "Are you sure you want to delete this review? This action cannot be undone."
            wrapMode: Text.WordWrap
            anchors.fill: parent
            anchors.margins: 20
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 14
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
        anchors.bottomMargin: root.paginationScale * 0.01
        spacing: root.width * 0.01
        visible: root.hasPagination

        // ← Prev
        CustomButton {
            id: prevButton
            text: "<"
            enabled: root.viewModel && root.viewModel.currentPage > 1 && !root.viewModel.isLoading
            onClicked: root.viewModel.prevPage()
            implicitWidth: root.paginationScale * 0.09
            implicitHeight: root.paginationScale * 0.07
            fontSize: root.paginationScale * 0.035
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
                    ? root.paginationScale * 0.05
                    : root.paginationScale * 0.09
                implicitHeight: root.paginationScale * 0.07
                anchors.verticalCenter: parent.verticalCenter

                // Ellipsis
                Text {
                    anchors.centerIn: parent
                    visible: modelData === -1
                    text: "..."
                    font.family: root.fontName
                    font.pixelSize: root.paginationScale * 0.035
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
                    fontSize: root.paginationScale * 0.032
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
            implicitWidth: root.paginationScale * 0.09
            implicitHeight: root.paginationScale * 0.07
            fontSize: root.paginationScale * 0.035
            baseColor: nextButton.enabled ? "#8572af" : "#c4b8e0"
            hoverColor: "#7060a0"
            clickColor: "#5a4a8a"
            textColor: "white"
            radius: 6
        }
    }
}
