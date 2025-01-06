// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import LiQML
import QtQuick.Controls.Material
import QtQuick.Controls 6.8
import QtQuick.Layouts


Window {
    id: window
    width: Constants.width
    height: Constants.height

    flags: Qt.FramelessWindowHint | Qt.Window

    visible: true
    color: "transparent"

    FramelessWindow {
        id: screen01
        anchors.fill: parent
        windowRoot: window

        Drawer {
            id: drawer
            x: 100; y: 10
            width: 0.3 * parent.width
            height: parent.height - 20
            bottomPadding: 10
            leftPadding: 10
            topPadding: 10

            Label {
                text: "Content goes here!"
                anchors.centerIn: parent
            }
        }

        Item {
            id: rectangle

            // color: "#121212"
            anchors.fill: parent
            anchors.margins: 20
            anchors.leftMargin: 247
            anchors.rightMargin: 38
            anchors.topMargin: 157
            anchors.bottomMargin: 30
            // color: "white"
            Material.theme: Material.Light
            Material.accent: Material.Cyan

            CheckBox {
                id: checkBox
                x: 25
                y: 84
                text: qsTr("复选框")
            }

            ComboBox {
                id: comboBox
                x: 25
                y: 151
                width: 120
                height: 37
                flat: true
                editable: true
                model: ["First", "Second", "Third"]
            }

            Switch {
                id: _switch
                x: 27
                y: 212
                text: qsTr("Switch")
            }

            SpinBox {
                id: spinBox
                x: 191
                y: 151
                width: 143
                height: 31
            }

            ToolSeparator {
                id: toolSeparator
                x: 277
                y: 193
                width: 13
                height: 227
            }
        }

        Button {
            id: button
            x: 275
            y: 175
            text: qsTr("Button")

            onClicked: drawer.open()
        }

        Image {
            id: image
            x: 20
            y: 18
            width: 60
            height: 45
            source: "image/icons8-linux-96.png"
            fillMode: Image.PreserveAspectFit
        }


    }

    property bool isMinimized: false

    /* 解决窗口最小化恢复后，左上角的标题阴影 */
    onVisibilityChanged: {
        if(window.visibility === Window.Minimized){
            window.isMinimized = true
            window.flags = Qt.Window
        } else if(window.isMinimized === true){
            window.flags = Qt.Window | Qt.FramelessWindowHint
            window.isMinimized = false
        }
    }

}

