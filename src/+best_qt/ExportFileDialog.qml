import QtQuick 2.15
import QtQuick.Dialogs 1.3

FileDialog {
    required property var proc
    id: root
    title: "Export results to which file"
    selectExisting: false
    nameFilters: [ "text file (*.txt)", "All files (*)" ]
    onAccepted: {
        proc.exportToText(root.fileUrl)
    }
}
