import QtQuick 2.15
import QtQuick.Dialogs

FileDialog {
    required property var proc
    id: root
    title: "Export results to which file"
    fileMode: FileDialog.SaveFile
    nameFilters: [ "text file (*.txt)", "All files (*)" ]
    onAccepted: {
        proc.exportToText(root.selectedFile)
    }
}
