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

        Rectangle {
            id: windowContent
            color: "transparent"

            anchors.fill: parent.windowContent

            Pane {
                id: pane
                x: 32
                y: 37
                width: 200
                height: 200



                CheckBox {
                    id: checkBox
                    x: 10
                    y: 10
                    text: qsTr("复选框")

                    // Material.theme: Material.Dark
                    Material.foreground: Material.Pink
                    Material.background: Material.Blue
                    Material.accent: Material.Cyan
                }
            }




            ComboBox {
                id: comboBox
                x: 378
                y: 81
                width: 120
                height: 37
                flat: true
                editable: true
                model: ["First", "Second", "Third"]
            }

            Switch {
                id: _switch
                x: 400
                y: 328
                text: qsTr("Switch")
            }

            SpinBox {
                id: spinBox
                x: 389
                y: 139
                width: 143
                height: 31
            }


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

