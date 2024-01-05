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


import org.raymii.DomainsListTextFile 1.0
import org.raymii.BrowserHistoryDB 1.0
import org.raymii.CAConcurrentGatherer 1.0
import org.raymii.VersionCheck 1.0
import SortFilterProxyModel 0.2

import "compat" as C

Window {
    width: 1400
    height: 1000
    visible: true
    x: 100
    y: 100
    title: qsTr("Which Root Certificates should you trust? by Remy van Elst")


    Rectangle {
        id: newVersionBar
        color: "#abd378"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        property bool shown: versionCheck.newVersionAvailable
        visible: height > 0
        height: shown ? 40 : 0
        Behavior on height {
            NumberAnimation {
                easing.type: Easing.InOutQuad
            }
        }
        clip: true

        Text {
            id: updateText
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            anchors.fill: parent
            text: "New version available. Click the here to see what's new."
            color: "black"
            font.pixelSize: 18
            minimumPixelSize: 12
            fontSizeMode: Text.Fit
            wrapMode: Text.WordWrap
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                newVersionBar.visible = false
                Qt.openUrlExternally("https://raymii.org")
            }
        }
    }

    TabBar {
        id: bar
        anchors.top: newVersionBar.visible ? newVersionBar.bottom : parent.top
        anchors.left: parent.left
        width: parent.width
        height: 40
        z: 2
        Repeater {
            model: ["Certificate Info", "Help"]
            TabButton {
                text: modelData
                width: Math.max(200, bar.width / 2)
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
                anchors.top: openChromeDBButton.bottom
                anchors.left: parent.left
                anchors.margins: 5
                width: 300
                text: "Open Text file (1 domain per line)"
                enabled: !proc.busy
                onClicked: textFileDialog.open()
            }

            Button {
                id: startButton
                anchors.top: parent.top
                anchors.left: openFFDBButton.right
                anchors.margins: 5
                width: 300
                text: proc.busy ? "STOP" : "START"

                onClicked: proc.startGatherCertificatesInBackground()

                contentItem: Text {
                    text: startButton.text
                    font: startButton.font
                    opacity: enabled ? 1.0 : 0.3
                    color: !proc.busy ? startButton.down ? "#17a81a" : "#21be2b" : "crimson"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideNone
                }
            }

            Button {
                id: exportButton
                anchors.top: startButton.bottom
                anchors.left: openFFDBButton.right
                anchors.margins: 5
                width: 300
                text: "Export Results"
                enabled: !proc.busy && proc.issuersCounted.rowCount > 0

                onClicked: exportFileDialog.open();

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
                text: proc.statusText === "" ? "Please close your browser before opening history file" : proc.statusText
            }

            TextField {
                id: search
                anchors.top: exportButton.bottom
                anchors.left: openChromeDBButton.right
                height: openFFDBButton.height
                width: openFFDBButton.width
                placeholderText: "Search"
                anchors.margins: 5
            }

            ProgressBar {
                id: prgbr
                anchors.top: startButton.bottom
                anchors.left: search.right
                width: 250
                anchors.margins: 5
                from: 0
                value: proc.progress
                to: 100
                padding: 2
            }

            Text {
                id: domainsHeader
                anchors.top: openTxtButton.bottom
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
                reuseItems: true
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
                spacing: 2                
                reuseItems: true
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
                    }
                }
            }

            Text {
                id: systemCAsNotInUseHeader
                anchors.top: search.bottom
                anchors.left: intCAHeader.right
                width: 500
                height: 25
                anchors.margins: 5
                text: "Unused System Root CA's (" + proc.notInUseSystemRootCAs.rowCount + ")"
                font.pixelSize: 20
            }

            ListView {
                id: systemCAsNotInUse
                anchors.top: systemCAsNotInUseHeader.bottom
                anchors.left: issuers.right
                anchors.bottom: parent.bottom
                width: 500
                anchors.margins: 5
                reuseItems: true
                spacing: 2
                model: unusedRootCAListProxy

                header: Text {
                    text: "When finished, this column holds all \nROOT CA's that were not used by the domains you requested."
                    width: systemCAsNotInUse.width
                    font.family: "Courier New"
                    wrapMode: Text.WordWrap
                }

                delegate: FoldableCertInfo {
                    modelData: model
                    titleBorderColor: modelData.index % 2 ? "black" : "#17a81a"
                    title: modelData.subject
                    width: systemCAsNotInUse.width
                    TextEdit {
                        readOnly: true
                        width: systemCAsNotInUse.width
                        font.family: "Courier New"
                        wrapMode: Text.WordWrap
                        selectByMouse: true
                        text: modelData.string
                    }
                }
            }

            Text {
                id: leafCertHeader
                anchors.top: search.bottom
                anchors.left: systemCAsNotInUseHeader.right
                width: 500
                height: 25
                anchors.margins: 5
                text: "Leaf Certificates (" + leafListProxy.count + ")"
                font.pixelSize: 20
            }

            ListView {
                id: leafCerts
                anchors.top: leafCertHeader.bottom
                anchors.left: systemCAsNotInUse.right
                anchors.bottom: parent.bottom
                width: 500
                anchors.margins: 5
                clip: true
                spacing: 2                
                reuseItems: true
                model: leafListProxy
                delegate: FoldableCertInfo {
                    modelData: model
                    titleBorderColor: modelData.index % 2 ? "black" : "#17a81a"
                    title: modelData.subjectInfo.commonName !== "" ? modelData.count + ": " + modelData.subjectInfo.commonName : modelData.count + ": " + modelData.subject
                    width: leafCerts.width
                    TextEdit {
                        readOnly: true
                        width: leafCerts.width
                        font.family: "Courier New"
                        wrapMode: Text.WordWrap
                        selectByMouse: true
                        text: modelData.string
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
                    }
                }
            }




        }

        ScrollView {
            id: helpTab
            contentWidth: availableWidth
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            width: parent.width
            height: parent.height
            clip: true

            Text {
                width: parent.width
                wrapMode: Text.WordWrap
                font.pixelSize: 14

                horizontalAlignment: Text.AlignHCenter
                textFormat: TextEdit.MarkdownText
                text: "# Which Root Certificate Authorities should you trust?

Gathers all certificates from your browser history (or list of domains).

**You should update your root store to trust only the root CA's for sites you actually visit.**

All CA's can sign certs for all domains, as was demonstrated by the Diginotar hack<p />

and the China Internet Network Information Center (CNNIC) & WoSign (China's

largest CA) issuance of fake certificates.<p />

This tool parses your browser history and retreives all certificates from all domains

in your history. No data is sent to a third-party service except for the GET request

to said domain. All this is done locally on your own device.

HTTP requests are sent 10 at once.

When finished, in the *Untrusted System Root CA's* column, you'll find

a list of system ROOT CA's that were not in use on the sites you visited.

Excluding USB and other local host related certificates, all those certs

should be safe to remove from your root store.

# Why did you make this tool

In the Twit.tv podcast Security Now, episode #951 it was stated that just 7

certificate authorities in total (of around 85) account for 99% of all

currently (late 2023) unexpired web certificates. Let's Encrypt is at almost

half of that (47%) followed up by DigiCert (22%) then Sectigo (former Comodo, 11%).


Having worked for a Dutch Certificate Authority myself (DigiDentity, not

DigiNotar!) I know a bit about certificates, combined with my programming

experience with Qt and some free time made for this tool. It should help

you clean up your root store and have only trusted root CA's for sites

that you actually visit. Probably not the Hong Kong Post office.


<hr/>

For me personally, the above numbers line up. Let's Encrypt is by far the

largest issuer of certificates to sites I visit. Followed up by a mix of

DigiCert, Comodo, VeriSign and QuoVadis. Surprises for me were the

`RO certSIGN certSIGN ROOT CA` (for the `community.ns.nl` domain) and

`TW TAIWAN-CA Root CA TWCA Global Root CA` (for `realtek.com`).

# Where is my Firefox history?

The path to your firefox profile folder which contains the history file

named `places.sqlite` is: `%APPDATA%\Mozilla\Firefox\Profiles\`.

Via firefox you can click the `menu` button, click `Help` and select

`More Troubleshooting Information`.

Under the `Application Basics` section next to `Profile Folder`, click `Open Folder`.

<hr/>

# Where is my chrome history?

The path to your Chrome profile folder which contains the history file

named `history` (no extension) is: `%LOCALAPPDATA%\Google\Chrome\User Data`.


Via Chrome you can navigate to `chrome://version`, then look for

the `Profile Path`


# A list of domains

You can use a text file with one domain (no `https://` in front,

no path or whatsoever at the back, just the domain) per line.

This file will be sorted and the domains in it queried just as the

browser history would.

# License and more information

License: GNU GPLv3
<p/>

Author: Remy van Elst (https://raymii.org).
<p/>

Source: https://github.com/RaymiiOrg/CertInfo
<p/>

The app icon is from the KDE Breeze Icon Theme and is licensed under the

GNU Lesser General Public License v2.1

אֶשָּׂא עֵינַי אֶל־הֶהָרִים מֵאַיִן יָבֹא עֶזְרִֽי׃


---
"                
            }
        }

    }

    SortFilterProxyModel {
        id: rootCAListProxy
        sourceModel: proc.issuersCounted
        sorters: RoleSorter { roleName: "count"; sortOrder: Qt.DescendingOrder}
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
        sorters: RoleSorter { roleName: "count"; sortOrder: Qt.DescendingOrder}

        delayed: true
        filters: [
            AllOf {
                RegExpFilter {
                    roleName: "subject"
                    pattern: search.text
                    caseSensitivity: Qt.CaseInsensitive
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
        delayed: true
        sorters: RoleSorter { roleName: "count"; sortOrder: Qt.DescendingOrder}

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
        delayed: true
        sorters: RoleSorter { roleName: "count"; sortOrder: Qt.DescendingOrder}

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
        id: unusedRootCAListProxy
        sourceModel: proc.notInUseSystemRootCAs
        delayed: true
        filters: [
            AllOf {
                RegExpFilter {
                    roleName: "subject"
                    pattern: "USB"
                    caseSensitivity: Qt.CaseInsensitive
                    inverted: true
                }
            }
        ]
    }

    SortFilterProxyModel {
        id: untrustedListProxy
        sourceModel: proc.issuersCounted
        delayed: true
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

    // ADD SYSTEM ROOT CA LIST AND REMOVE ALL FOUND CERTS,
    // LEAVING ONLY A LIST OF UNUSED ROOT CA'S

    DomainsListTextFile {
        id: txt       
    }

    BrowserHistoryDB {
        id: db
    }

    CAConcurrentGatherer {
        id: proc;
        hostnames: db.domains.rowCount === 0 ? txt.hostnames : db.hostnames
    }

    C.ImportHostsFileDialog {
        id: firefoxFileDialog
        isFirefox: true
        isTextFile: false
        db: db
        txt: txt
    }

    C.ImportHostsFileDialog {
        id: chromeFileDialog
        isFirefox: false
        isTextFile: false
        db: db
        txt: txt
    }

    C.ImportHostsFileDialog {
        id: textFileDialog
        isTextFile: true
        isFirefox: false
        db: db
        txt: txt
    }

    C.ExportFileDialog {
        id: exportFileDialog;
        proc: proc
    }

    VersionCheck {
        id: versionCheck
    }


}


