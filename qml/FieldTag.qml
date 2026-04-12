import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Control {
    id: root

    property string labelText: "Animal Filters"
    property var availableItems: null  // ListModel with dtoField, dtoValue, displayName
    property var selectedItems: null   // ListModel with same structure
    
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

    implicitWidth: 720
    implicitHeight: contentColumn.implicitHeight + topPadding + bottomPadding
    padding: 8

    readonly property var selectedFilters: collectSelectedFilters()

    function normalizeText(value) {
        if (value === undefined || value === null) {
            return ""
        }
        return String(value).toLowerCase()
    }

    function containsSelected(dtoField, dtoValue) {
        var fieldText = String(dtoField)
        var valueText = String(dtoValue)
        var model = root.selectedItems || selectedFiltersModel
        for (var i = 0; i < model.count; ++i) {
            var current = model.get(i)
            if (String(current.dtoField) === fieldText && String(current.dtoValue) === valueText) {
                return true
            }
        }
        return false
    }

    function collectSelectedFilters() {
        var model = root.selectedItems || selectedFiltersModel
        var data = []
        for (var i = 0; i < model.count; ++i) {
            data.push(model.get(i))
        }
        return data
    }

    function refreshSuggestions() {
        filteredSuggestionsModel.clear()

        var availableModel = root.availableItems || availableFiltersModel
        var query = normalizeText(searchField.text).trim()
        for (var i = 0; i < availableModel.count; ++i) {
            var option = availableModel.get(i)
            if (containsSelected(option.dtoField, option.dtoValue)) {
                continue
            }

            var searchableText = String(option.displayName).toLowerCase()
            if (query.length === 0 || searchableText.indexOf(query) !== -1) {
                filteredSuggestionsModel.append({
                    dtoField: option.dtoField,
                    dtoValue: option.dtoValue,
                    displayName: option.displayName
                })
            }
        }
    }

    function openSuggestions() {
        refreshSuggestions()
        if (!suggestionsPopup.opened) {
            suggestionsPopup.open()
        }
    }

    function addSelected(dtoField, dtoValue, displayName) {
        if (containsSelected(dtoField, dtoValue)) {
            return
        }

        var model = root.selectedItems || selectedFiltersModel
        model.append({
            dtoField: dtoField,
            dtoValue: dtoValue,
            displayName: displayName
        })

        searchField.text = ""
        refreshSuggestions()
        searchField.forceActiveFocus()
        suggestionsPopup.open()
    }

    function removeSelected(index) {
        var model = root.selectedItems || selectedFiltersModel
        if (index < 0 || index >= model.count) {
            return
        }

        model.remove(index)
        refreshSuggestions()
    }

    function pickCurrentSuggestion() {
        if (!suggestionsPopup.opened || suggestionsList.currentIndex < 0 || suggestionsList.currentIndex >= filteredSuggestionsModel.count) {
            return
        }

        var selectedItem = filteredSuggestionsModel.get(suggestionsList.currentIndex)
        addSelected(selectedItem.dtoField, selectedItem.dtoValue, selectedItem.displayName)
    }

    ListModel {
        id: availableFiltersModel
    }

    ListModel {
        id: selectedFiltersModel

    }

    ListModel {
        id: filteredSuggestionsModel
    }

    Component.onCompleted: refreshSuggestions()

    background: Rectangle {
        radius: 14
        color: theme.pageBg
        border.color: theme.purple
        border.width: 1
    }

    contentItem: ColumnLayout {
        id: contentColumn
        spacing: 6

        Text {
            text: root.labelText
            font.family: theme.fontName
            font.pixelSize: 14
            font.bold: true
            color: theme.textDark
        }

        Rectangle {
            id: inputShell
            Layout.fillWidth: true
            radius: 10
            color: theme.fieldBg
            border.color: theme.purple
            border.width: 1
            implicitHeight: Math.max(40, chipsFlow.implicitHeight + 12)

            TapHandler {
                onTapped: {
                    searchField.forceActiveFocus()
                    root.openSuggestions()
                }
            }

            Flow {
                id: chipsFlow
                anchors.fill: parent
                anchors.margins: 8
                spacing: 6

                Repeater {
                    model: root.selectedItems || selectedFiltersModel

                    delegate: Rectangle {
                        radius: 14
                        height: 28
                        color: theme.purple
                        border.color: theme.accentPink
                        border.width: 1

                        implicitWidth: chipRow.implicitWidth + 12

                        Row {
                            id: chipRow
                            anchors.centerIn: parent
                            spacing: 6

                            Text {
                                text: displayName
                                color: theme.buttonText
                                font.family: theme.fontName
                                font.pixelSize: 13
                            }

                            ToolButton {
                                text: "×"
                                padding: 0
                                font.pixelSize: 14
                                width: 16
                                height: 16
                                onClicked: root.removeSelected(index)
                                background: Rectangle {
                                    color: "transparent"
                                }
                            }
                        }
                    }
                }

                TextField {
                    id: searchField
                    width: 180
                    height: 28
                    placeholderText: "Search filters"
                    placeholderTextColor: theme.accentPink
                    color: theme.textDark
                    font.family: theme.fontName
                    selectByMouse: true
                    background: Rectangle {
                        color: "transparent"
                    }

                    onTextChanged: root.openSuggestions()
                    onActiveFocusChanged: {
                        if (activeFocus) {
                            root.openSuggestions()
                        }
                    }

                    Keys.onDownPressed: {
                        if (!suggestionsPopup.opened) {
                            root.openSuggestions()
                        }
                        if (suggestionsList.count > 0) {
                            suggestionsList.currentIndex = Math.min(suggestionsList.count - 1, suggestionsList.currentIndex + 1)
                        }
                    }

                    Keys.onUpPressed: {
                        if (suggestionsList.count > 0) {
                            suggestionsList.currentIndex = Math.max(0, suggestionsList.currentIndex - 1)
                        }
                    }

                    Keys.onReturnPressed: root.pickCurrentSuggestion()
                    Keys.onEnterPressed: root.pickCurrentSuggestion()
                    Keys.onEscapePressed: suggestionsPopup.close()
                }
            }
        }

        Popup {
            id: suggestionsPopup
            x: 0
            y: inputShell.y + inputShell.height + root.height * 0.015
            width: contentColumn.width
            padding: 6
            modal: false
            focus: false
            closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
            parent: contentColumn

            onOpened: {
                suggestionsList.currentIndex = filteredSuggestionsModel.count > 0 ? 0 : -1
            }

            background: Rectangle {
                radius: 10
                color: theme.fieldBg
                border.color: theme.purple
                border.width: 1
            }

            contentItem: Column {
                spacing: 0

                ListView {
                    id: suggestionsList
                    width: parent.width
                    clip: true
                    implicitHeight: Math.min(contentHeight, 220)
                    model: filteredSuggestionsModel

                    delegate: ItemDelegate {
                        width: suggestionsList.width
                        text: displayName
                        background: Rectangle {
                            color: hovered ? theme.purple : "transparent"
                        }
                        contentItem: Text {
                            text: parent.text
                            color: parent.hovered ? theme.buttonText : theme.textDark
                            font.family: theme.fontName
                            font.pixelSize: 13
                        }

                        onClicked: {
                            root.addSelected(dtoField, dtoValue, displayName)
                        }
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 40
                    color: "transparent"
                    visible: filteredSuggestionsModel.count === 0

                    Text {
                        anchors.centerIn: parent
                        text: "No matches"
                        color: theme.textDark
                        font.family: theme.fontName
                        font.pixelSize: 13
                    }
                }
            }
        }
    }
}
