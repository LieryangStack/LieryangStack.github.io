// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import LiQML
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Shapes
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

        ComboBox {
            x: 488
            y: 93

            model: ["人体识别", "姿态识别", "3D分割1111111", "张三", "李四", "王五"]

        }

        SwitchDelegate {
            id: switchDelegate
            x: 151
            y: 359
            text: qsTr("Switch Delegate")
        }

        ComboBox {
            id: comboBox
            x: 440
            y: 240
            width: 155
            height: 40

            model: ["人体识别", "姿态识别", "3D分割1111111", "张三", "李四", "王五"]

            indicator: Image {
                id: arrow
                x: comboBox.width - arrow.width
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 5
                source: "image/li-arrow-down.svg"
                sourceSize.width: 25
                sourceSize.height: 20
                rotation: 0
            }

            /* 自定义委托，用于显示下拉框中的每一项 */
            delegate: ItemDelegate {
                id: delegate
                width: popup.width - 20
                required property var model
                required property int index
                highlighted: comboBox.highlightedIndex === index
                contentItem: Text {
                    color: "#000000"
                    text: delegate.model[comboBox.textRole]
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font: comboBox.font
                }
            }

            /* 自定义内容项，用于显示当前选中的文本 */
            contentItem: Text {
                color: comboBox.pressed ? "#17a81a" : "black"
                text: comboBox.displayText
                elide: Text.ElideRight
                anchors.left: parent.left
                anchors.right: arrow.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                font.weight: Font.Normal
                font.strikeout: false
                font.pointSize: 10
                font.bold: false
            }

            background: Rectangle {
                color: "transparent"
                radius: 10
                border.color: "gray"
                border.width: 1
                anchors.fill: parent
                smooth: true
            }

            Layout.preferredWidth: 129
            Layout.preferredHeight: 26

            popup: Popup {
                id: popup
                y: parent.height + 5
                width: comboBox.width
                height: 200
                implicitHeight: listview1.contentHeight

                contentItem: ListView {
                    id: listview1
                    anchors.fill: parent
                    anchors.margins: 5
                    model: comboBox.popup.visible ? comboBox.delegateModel : null
                    currentIndex: comboBox.highlightedIndex
                    clip: true

                    ScrollIndicator.vertical: ScrollIndicator {
                    }
                }

                background: Rectangle {
                    id: backgroundItem
                    color: "transparent"

                    ShaderEffectSource {
                        id: windowSource
                        visible: false
                        anchors.fill: parent
                        sourceRect: Qt.rect (comboBox.x + popup.x, comboBox.y + popup.y, popup.width, popup.height)
                        sourceItem: screen01
                    }

                    Rectangle {
                        id: source1
                        anchors.fill: parent
                        anchors.margins: 50
                        visible: false
                        radius: 10
                    }


                    /* 模糊 */
                   GaussianBlur {
                       id: blur
                        anchors.fill: source1
                        source: windowSource
                        radius: 15
                        deviation: 500
                        samples: 100
                   }

                    OpacityMask {
                        visible: true
                        anchors.fill: source1
                        source: blur
                        maskSource: source1
                    }

                    // Rectangle {
                    //     id: source2
                    //     opacity: 0.2
                    //     anchors.fill: parent
                    //     radius: 10
                    // }


                }
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

