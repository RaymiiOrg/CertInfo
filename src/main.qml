/*
 * Copyright (c) 2023 Remy van Elst https://raymii.org
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.0

import org.raymii.DomainsListTextFile 1.0
import org.raymii.BrowserHistoryDB 1.0
import org.raymii.CAConcurrentGatherer 1.0
import SortFilterProxyModel 0.2

Window {
    width: 1400
    height: 1000
    visible: true
    x: 100
    y: 100
    title: qsTr("Which Root Certificates should you trust? by Remy van Elst")


    TabBar {
        id: bar
        width: parent.width
        height: 40
        z: 2
        Repeater {
            model: ["Certificate Info", "Help"]
            TabButton {
                text: modelData
                width: Math.max(100, bar.width / model.length)
            }
        }
    }


    StackLayout {
        anchors.top: bar.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        currentIndex: bar.currentIndex

        ScrollView {
            id: homeTab
            width: parent.width
            height: parent.height
            clip: true
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOn
            contentWidth: contentItem.children[0].childrenRect.width

            Button {
                id: openFFDBButton
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.margins: 5
                width: 300
                enabled: !proc.busy
                text: "Open Firefox History file (places.sqlite)"
                onClicked: firefoxFileDialog.open()
            }

            Button {
                id: openChromeDBButton
                anchors.top: openFFDBButton.bottom
                anchors.left: parent.left
                anchors.margins: 5
                width: 300
                enabled: !proc.busy
                text: "Open Chrome History file (history)"
                onClicked: chromeFileDialog.open()
            }

            Button {
                id: openTxtButton
                anchors.top: parent.top
                anchors.left: openFFDBButton.right
                anchors.margins: 5
                width: 300
                text: "Open Text file (1 domain per line)"
                enabled: !proc.busy
                onClicked: textFileDialog.open()
            }

            Button {
                id: startButton
                anchors.top: parent.top
                anchors.left: openTxtButton.right
                anchors.margins: 5
                width: 250
                text: "START"
                enabled: !proc.busy
                onClicked: proc.startGatherCertificatesInBackground()
            }

            Text {
                id: infoText
                anchors.left: startButton.right
                anchors.right: parent.right
                height: 20
                anchors.top: parent.top
                anchors.margins: 5
                font.pixelSize: 12
                text: db.dbFileName === "" ? txt.lastError === "" ? txt.textFileName : txt.lastError : db.lastDbError === "" ? db.dbFileName : db.lastDbError
            }

            Text {
                id: statusText
                anchors.left: startButton.right
                anchors.right: parent.right
                height: 20
                anchors.top: infoText.bottom
                anchors.margins: 5
                font.pixelSize: 12
                text: proc.statusText
            }

            TextField {
                id: search
                anchors.top: openFFDBButton.bottom
                anchors.left: openChromeDBButton.right
                height: openFFDBButton.height
                width: openFFDBButton.width
                placeholderText: "Search"
                anchors.margins: 5
            }

            ProgressBar {
                id: prgbr
                anchors.top: openFFDBButton.bottom
                anchors.left: search.right
                width: 250
                anchors.margins: 5
                from: 0
                value: proc.progress
                to: 100
                padding: 2
                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 6
                    color: "#e6e6e6"
                    radius: 3
                }

                contentItem: Item {
                    implicitWidth: 200
                    implicitHeight: 4

                    Rectangle {
                        width: prgbr.visualPosition * parent.width
                        height: parent.height
                        radius: 2
                        color: "#17a81a"
                    }
                }
            }

            Text {
                id: domainsHeader
                anchors.top: search.bottom
                anchors.left: parent.left
                height: 25
                width: 300
                anchors.margins: 5
                text: db.domains.rowCount === 0 ? "Domains (" + txt.domains.rowCount + ")" : "Domains (" + db.domains.rowCount + ")"
                font.pixelSize: 20
            }



            ListView {
                id: hostnames
                anchors.top: domainsHeader.bottom
                anchors.left: parent.left
                height: (parent.height / 3) * 2 - 100
                width: 300
                anchors.margins: 5
                clip: true
                model: db.domains.rowCount === 0 ? txt.domains : db.domains

                delegate: Text {
                    text: model.count + ": " + model.domain
                    width: 300
                    color: index % 2 ? "black" : "#17a81a"
                    minimumPixelSize: 12
                    fontSizeMode: Text.Fit
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    elide: Text.ElideMiddle
                }
            }


            Text {
                id: errorsHeader
                anchors.top: hostnames.bottom
                anchors.left: parent.left
                width: 300
                height: 25
                anchors.margins: 5
                text: "Errors"
                font.pixelSize: 20
            }

            ListView {
                id: errors
                anchors.top: errorsHeader.bottom
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                width: 300
                anchors.margins: 5
                clip: true
                model: errorListProxy
                delegate: Text {
                    width: 300
                    font.family: "Courier New"
                    minimumPixelSize: 12
                    fontSizeMode: Text.Fit
                    elide: Text.ElideMiddle
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    text: model.subject
                    color: index % 2 ? "black" : "#17a81a"
                }
            }


            Text {
                id: rootCAHeader
                anchors.top: search.bottom
                anchors.left: domainsHeader.right
                width: 500
                height: 25
                anchors.margins: 5
                text: "Root CA's (" + rootCAListProxy.count + ")"

                font.pixelSize: 20
            }


            ListView {
                id: rootCertificates
                anchors.top: domainsHeader.bottom
                anchors.left: hostnames.right
                anchors.bottom: parent.bottom
                width: 500
                anchors.margins: 5
                clip: true
                spacing: 2
                model: rootCAListProxy
                delegate: FoldableCertInfo {
                    modelData: model
                    titleBorderColor: modelData.index % 2 ? "black" : "#17a81a"
                    title: modelData.count + ": " + modelData.subject
                    width: rootCertificates.width
                    TextEdit {
                        readOnly: true
                        width: rootCertificates.width
                        font.family: "Courier New"
                        wrapMode: Text.WordWrap
                        selectByMouse: true
                        text: modelData.string
                        //text: model.selfsigned ? model.count + ": " + model.subject + " (SELF SIGNED)" : model.count + ": " + model.subject + " (" + model.issuer + ")"
                    }
                }
            }

            Text {
                id: intCAHeader
                anchors.top: search.bottom
                anchors.left: rootCAHeader.right
                width: 500
                height: 25
                anchors.margins: 5
                text: "Intermediate CA's (" + regularCAListProxy.count + ")"
                font.pixelSize: 20
            }

            ListView {
                id: issuers
                anchors.top: intCAHeader.bottom
                anchors.left: rootCertificates.right
                anchors.bottom: parent.bottom
                width: 500
                anchors.margins: 5
                clip: true
                spacing: 2
                model: regularCAListProxy
                delegate: FoldableCertInfo {
                    modelData: model
                    titleBorderColor: modelData.index % 2 ? "black" : "#17a81a"
                    title: modelData.count + ": " + modelData.subject
                    width: issuers.width
                    TextEdit {
                        readOnly: true
                        width: issuers.width
                        font.family: "Courier New"
                        wrapMode: Text.WordWrap
                        selectByMouse: true
                        text: modelData.string
                        //text: model.selfsigned ? model.count + ": " + model.subject + " (SELF SIGNED)" : model.count + ": " + model.subject + " (" + model.issuer + ")"
                    }
                }
            }

            Text {
                id: leafCertHeader
                anchors.top: search.bottom
                anchors.left: intCAHeader.right
                width: 500
                height: 25
                anchors.margins: 5
                text: "Leaf Certificates (" + leafListProxy.count + ")"
                font.pixelSize: 20
            }

            ListView {
                id: leafCerts
                anchors.top: leafCertHeader.bottom
                anchors.left: issuers.right
                anchors.bottom: parent.bottom
                width: 500
                anchors.margins: 5
                clip: true
                spacing: 2
                model: leafListProxy
                delegate: FoldableCertInfo {
                    modelData: model
                    titleBorderColor: modelData.index % 2 ? "black" : "#17a81a"
                    title: modelData.subjectInfo.commonName !== "" ? modelData.count + ": " + modelData.subjectInfo.commonName : modelData.subject
                    width: leafCerts.width
                    TextEdit {
                        readOnly: true
                        width: leafCerts.width
                        font.family: "Courier New"
                        wrapMode: Text.WordWrap
                        selectByMouse: true
                        text: modelData.string
                        //text: model.selfsigned ? model.count + ": " + model.subject + " (SELF SIGNED)" : model.count + ": " + model.subject + " (" + model.issuer + ")"
                    }
                }
            }


            Text {
                id: untrustedHeader
                anchors.top: search.bottom
                anchors.left: leafCertHeader.right
                width: 500
                height: 25
                anchors.margins: 5
                text: "Untrusted (" + untrustedListProxy.count + ")"
                font.pixelSize: 20
            }

            ListView {
                id: untrusted
                anchors.top: untrustedHeader.bottom
                anchors.left: leafCerts.right
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                width: 500
                anchors.margins: 5
                spacing: 2
                clip: true
                model: untrustedListProxy

                delegate: FoldableCertInfo {
                    modelData: model
                    titleBorderColor: modelData.index % 2 ? "black" : "#17a81a"
                    title: modelData.count + ": " + modelData.subject
                    width: untrusted.width
                    TextEdit {
                        readOnly: true
                        width: untrusted.width
                        font.family: "Courier New"
                        wrapMode: Text.WordWrap
                        selectByMouse: true
                        text: modelData.string
                        //text: model.selfsigned ? model.count + ": " + model.subject + " (SELF SIGNED)" : model.count + ": " + model.subject + " (" + model.issuer + ")"
                    }
                }
            }




        }

        ScrollView {
            id: helpTab
            contentWidth: availableWidth
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            contentHeight: contentItem.children[0].childrenRect.height

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 5


                Text {
                    Layout.preferredWidth: parent.width
                    wrapMode: Text.WordWrap
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignLeft
                    textFormat: TextEdit.MarkdownText
                    text: "Gathers all certificates from your browser history (or list of domains).

**You should update your root store to trust only the root certificates that sites you actually visited use.**

All CA's can sign certs for all domains, as was demonstrated by the Diginotar hack<p />

and the China Internet Network Information Center (CNNIC) & WoSign (China's largest CA) issuance of fake certificates.

License: GNU GPLv3
<p/>

Author: Remy van Elst (https://raymii.org).
<p/>

Source: https://github.com/RaymiiOrg/YouLessQt
</p>

אֶשָּׂא עֵינַי אֶל־הֶהָרִים מֵאַיִן יָבֹא עֶזְרִֽי׃


---
"
                }
            }
        }

    }

    SortFilterProxyModel {
        id: rootCAListProxy
        sourceModel: proc.issuersCounted
        filters: [
            AllOf {
                ValueFilter {
                    enabled: true
                    roleName: "istrustedrootca"
                    value: true
                }
                ValueFilter {
                    enabled: true
                    roleName: "errors"
                    value: ""
                }
                RegExpFilter {
                    roleName: "subject"
                    pattern: search.text
                    caseSensitivity: Qt.CaseInsensitive
                }
            }
        ]
    }

    SortFilterProxyModel {
        id: regularCAListProxy
        sourceModel: proc.issuersCounted
        filters: [
            AllOf {
                AnyOf {
                    RegExpFilter {
                        roleName: "subject"
                        pattern: search.text
                        caseSensitivity: Qt.CaseInsensitive
                    }
                    RegExpFilter {
                        roleName: "domains"
                        pattern: search.text
                        caseSensitivity: Qt.CaseInsensitive
                    }
                }
                ValueFilter {
                    enabled: true
                    roleName: "istrustedrootca"
                    value: false
                }
                ValueFilter {
                    enabled: true
                    roleName: "isca"
                    value: true
                }
                ValueFilter {
                    enabled: true
                    roleName: "isselfsigned"
                    value: false
                }
                ValueFilter {
                    enabled: true
                    roleName: "errors"
                    value: ""
                }
            }
        ]
    }

    SortFilterProxyModel {
        id: leafListProxy
        sourceModel: proc.issuersCounted
        filters: [
            AllOf {
                AnyOf {
                    RegExpFilter {
                        roleName: "subject"
                        pattern: search.text
                        caseSensitivity: Qt.CaseInsensitive
                    }
                    RegExpFilter {
                        roleName: "domains"
                        pattern: search.text
                        caseSensitivity: Qt.CaseInsensitive
                    }
                }
                ValueFilter {
                    enabled: true
                    roleName: "isca"
                    value: false
                }
                ValueFilter {
                    enabled: true
                    roleName: "errors"
                    value: ""
                }
            }
        ]
    }

    SortFilterProxyModel {
        id: errorListProxy
        sourceModel: proc.issuersCounted
        filters: [
            ValueFilter {
                enabled: true
                roleName: "errors"
                value: ""
                inverted: true
            }
        ]
    }

    SortFilterProxyModel {
        id: untrustedListProxy
        sourceModel: proc.issuersCounted
        filters: [
            AllOf {
                ValueFilter {
                    enabled: true
                    roleName: "istrustedrootca"
                    value: false
                }
                ValueFilter {
                    enabled: true
                    roleName: "isselfsigned"
                    value: true
                }
                ValueFilter {
                    enabled: true
                    roleName: "isca"
                    value: true
                }
                ValueFilter {
                    enabled: true
                    roleName: "errors"
                    value: ""
                }
            }
        ]
    }


    DomainsListTextFile {
        id: txt
        Component.onCompleted: {
            txt.getHostnamesFromTextFile("file:///C:/Users/Remy/domains.txt")
        }
    }

    BrowserHistoryDB {
        id: db
        // Component.onCompleted: {
        //     db.openDb("file:///C:/Users/Remy/places.sqlite")
        //     db.getHostnamesFromDb()
        // }
    }


    CAConcurrentGatherer {
        id: proc;
        hostnames: db.hostnames.length === 0 ? txt.hostnames : db.hostnames
    }

    FileDialog {
        id: firefoxFileDialog
        title: "Please choose the Firefox places.sqlite file"
        folder: db.firefoxFolder
        nameFilters: [ "places.sqlite (places.sqlite)", "All files (*)" ]
        onAccepted: {
            if(!db.openDb(firefoxFileDialog.fileUrl)) {
                infoText.text = "Error opening db file!"
            } else {
                txt.hostnames = []
                db.hostnames = []
                db.isFirefox = true;
                db.getHostnamesFromDb();
            }
        }
    }

    FileDialog {
        id: chromeFileDialog
        title: "Please choose the Chrome History file"
        folder: db.chromeFolder
        nameFilters: [ "History (History)", "All files (*)" ]
        onAccepted: {
            if(!db.openDb(chromeFileDialog.fileUrl)) {
                infoText.text = "Error opening db file!"
            } else {
                txt.hostnames = []
                db.hostnames = []
                db.isFirefox = false;
                db.getHostnamesFromDb();
            }
        }
    }

    FileDialog {
        id: textFileDialog
        title: "Choose a txt file with one domain per line"
        folder: shortcuts.home
        nameFilters: [ "text file (*.txt)", "All files (*)" ]
        onAccepted: {
            db.hostnames = []
            txt.hostnames = []
            txt.getHostnamesFromTextFile(textFileDialog.fileUrl)
        }
    }

}


