import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    width: parent ? parent.width : implicitWidth
    height: parent ? parent.height : implicitHeight

    property var viewModel: null
    property Component headerComponent: null
    property bool showPaginationControls: true

    readonly property color textDark: "#8572af"
    readonly property string fontName: "Comic Sans MS"

    readonly property bool hasPagination: viewModel && viewModel.totalPages > 1 && showPaginationControls

    ListView {
        id: postListView
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: hasPagination ? paginationRow.top : parent.bottom

        clip: true
        boundsBehavior: Flickable.StopAtBounds
        
        readonly property real scrollBarMargin: verticalScrollBar.width + 6

        header: headerComponent ? headerWrapper : null
        
        Component {
            id: headerWrapper
            Item {
                width: postListView.width - postListView.scrollBarMargin
                height: header.height + headerMargin
                
                Loader {
                    id: header
                    sourceComponent: headerComponent
                    width: parent.width
                }
                
                property real headerMargin: root.height * 0.012
            }
        }
        footer: (root.viewModel && root.viewModel.isLoading)
            ? loadingFooter : null
        
        Component {
            id: loadingFooter
            Item {
                width: postListView.width - postListView.scrollBarMargin
                implicitHeight: Math.min(root.width, root.height) * 0.2
                
                LoaderSpinner {
                    anchors.centerIn: parent
                    width: parent.width * 0.2
                    height: width
                    running: true
                    visible: true
                }
            }
        }

        spacing: root.height * 0.012
        delegate: PostCardView {
            width: postListView.width - postListView.scrollBarMargin
            postId: model.postId
            postText: model.postText
            postCreatedAt: model.postCreatedAt
        }

        ScrollBar.vertical: ScrollBar { 
            id: verticalScrollBar
            policy: ScrollBar.AsNeeded 
        }

        model: root.viewModel ? root.viewModel.listModel : null
    }

    function buildPageWindow() {
        if (!viewModel || viewModel.totalPages <= 1) return []
        const cur = viewModel.currentPage
        const last = viewModel.totalPages
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

    Row {
        id: paginationRow
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: root.height * 0.01
        spacing: root.width * 0.01
        visible: root.hasPagination

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

        Repeater {
            model: root.buildPageWindow()

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
                    font.family: root.fontName
                    font.pixelSize: Math.min(root.width, root.height) * 0.035
                    color: root.textDark
                }

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