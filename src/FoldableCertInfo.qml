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
import QtQuick.Controls 2.15

Column {
    id: root
    default property alias children: content.children
    property var modelData
    property string title: modelData.subject
    property color titleBgColor:  "#e0e0e0"
    property color titleBgColorPressed: "#cfcfcf"
    property color titleBorderColor: titleBgColor
    property bool collapsed: true

    Button {
        id: header
        anchors.left: parent.left
        width: parent.width
        clip: true
        height: 48
        contentItem: Text {
            height: parent.height
            anchors.margins: 5
            width: parent.width
            text: root.title
            horizontalAlignment : Text.AlignLeft
            minimumPixelSize: 12
            fontSizeMode: Text.Fit
            wrapMode: Text.WordWrap
            elide: Text.ElideMiddle
        }

        onClicked: root.collapsed = !root.collapsed
        background: Rectangle {
            color: parent.down ? titleBgColorPressed : titleBgColor
            border.color: titleBorderColor
            border.width: 1
            radius: 2
        }
    }

    Pane {
        id: contentPane

        visible: height > 0
        height: !root.collapsed ? implicitHeight : 0

        Behavior on height {
            NumberAnimation {
                easing.type: Easing.InOutQuad
                duration: 200
            }
        }

        clip: true
        anchors.left: parent.left
        anchors.right: parent.right

        Column {
            id: content
            anchors.right: parent.right
            anchors.left: parent.left
            spacing: 10
        }
    }
}
