import QtQuick 2.15
import QtQuick.Dialogs

FileDialog {
    property bool isFirefox: false
    property bool isTextFile: false
    required property var db
    required property var txt
    id: root
    title: isTextFile ? "Choose a txt file with one domain per line" : isFirefox ? "Please choose the Firefox places.sqlite file" : "Please choose the Chrome/Edge History file"
    nameFilters: isTextFile ?  [ "text file (*.txt)", "All files (*)" ] : isFirefox ? [ "places.sqlite (places.sqlite)", "All files (*)" ] : [ "History (History)", "All files (*)" ]
    onAccepted: {
        txt.hostnames = ""
        db.hostnames = ""
        if(root.isTextFile) {
            txt.getHostnamesFromTextFile(root.selectedFile)
        } else {
            if(!db.openDb(root.selectedFile)) {
                infoText.text = "Error opening db file!"
            } else {
                db.isFirefox = root.isFirefox;
                db.getHostnamesFromDb();
            }
        }
    }
}
