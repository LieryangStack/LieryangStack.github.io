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

            model: ["人体识别", "姿态识别", "3D分割1111111111111111111111", "张三", "李四", "王五"]

        }

        SwitchDelegate {
            id: switchDelegate
            x: 151
            y: 359
            text: qsTr("Switch Delegate")
        }

        Rectangle {
            id: rectangle
            x: 102
            y: 191
            width: 387
            height: 40
            color: "#ffffff"
            radius: 10

            Text {
                id: _text
                color: "#353434"
                text: "设置识别对象"
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 10
                font.pixelSize: 16
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                Layout.preferredWidth: 124
                Layout.preferredHeight: 36
            }

            ComboBox {
                id: comboBox
                x: 261
                width: 111
                height: 20
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 15

                model: ["人体识别", "姿态识别", "3D分割1111111111111111111111", "张三", "李四", "王五"]


                indicator: Image {
                    id: arrow
                    x: comboBox.width - arrow.width
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 5
                    source: "image/arrow-down-svgrepo-com.svg"
                    sourceSize.width: 25
                    sourceSize.height: 20
                    rotation: 0
                }

                delegate: ItemDelegate {
                    id: delegate
                    width: popup.width - 20
                    property var model
                    property int index
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

                contentItem: Text {
                    color: comboBox.pressed ? "#17a81a" : "black"
                    text: comboBox.displayText
                    elide: Text.ElideRight
                    anchors.left: parent.left
                    anchors.right: arrow.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.rightMargin: 5
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    font.weight: Font.Normal
                    font.strikeout: false
                    font.pointSize: 12
                    font.bold: true
                }

                background: Rectangle {
                    color: "#00000000"
                    anchors.fill: parent
                }

                Layout.preferredWidth: 129
                Layout.preferredHeight: 26

                popup: Popup {
                    id: popup
                    x: arrow.x + (arrow.width - width) / 2
                    y: arrow.height
                    width: comboBox.width
                    height: 200
                    property int myRadius: 15
                    implicitHeight: listview1.contentHeight
                    property int d: 15
                    contentItem: ListView {
                        id: listview1
                        anchors.fill: parent
                        anchors.leftMargin: 5
                        anchors.rightMargin: 5
                        anchors.topMargin: 15
                        anchors.bottomMargin: 5
                        model: comboBox.popup.visible ? comboBox.delegateModel : null
                        delegate: Item {
                            width: ListView.view.width
                            height: 50
                            Text {
                                text: model.name
                                anchors.centerIn: parent
                            }

                            Rectangle {
                                width: parent.width
                                height: parent.height
                                color: "#add8e6"
                                radius: 5
                            }
                        }
                        currentIndex: comboBox.highlightedIndex
                        clip: true
                        ScrollIndicator.vertical: ScrollIndicator {
                        }
                    }
                    background: Rectangle {
                        color: "#00000000"
                        anchors.fill: parent
                        layer.enabled: true
                        layer.effect: Glow {
                            color: "#808080"
                            radius: 8
                            anchors.fill: parent
                            spread: 0.1
                            samples: 20
                        }
                        Shape {
                            id: shape
                            anchors.fill: parent
                            smooth: true
                            preferredRendererType: Shape.CurveRenderer
                            ShaderEffectSource {
                                id: windowSource
                                visible: false
                                anchors.fill: shape
                                sourceRect: Qt.rect (rectangle.x + comboBox.x + popup.x, rectangle.x + comboBox.y + popup.y, shape.width, shape.height)
                                sourceItem: screen01
                            }

                            Rectangle {
                                id: blurRect
                                visible: false
                                anchors.fill: shape
                                FastBlur {
                                    radius: 50
                                    anchors.fill: blurRect
                                    source: windowSource
                                }
                            }

                            OpacityMask {
                                anchors.fill: blurRect
                                source: blurRect
                                maskSource: parent
                            }

                            ShapePath {
                                id: shapePath
                                strokeWidth: 1
                                strokeStyle: ShapePath.SolidLine
                                strokeColor: "#00000000"
                                startY: 0
                                startX: popup.width/2
                                PathLine {
                                    x: popup.width/2 + 15
                                    y: 10
                                }

                                PathLine {
                                    x: popup.width - 15
                                    y: 10
                                }

                                PathArc {
                                    x: popup.width
                                    y: 25
                                    radiusY: 15
                                    radiusX: 15
                                }

                                PathLine {
                                    x: popup.width
                                    y: popup.height - 15
                                }

                                PathArc {
                                    x: popup.width - 15
                                    y: popup.height
                                    radiusY: 15
                                    radiusX: 15
                                }

                                PathLine {
                                    x: 15
                                    y: popup.height
                                }

                                PathArc {
                                    x: 0
                                    y: popup.height -15
                                    radiusY: 15
                                    radiusX: 15
                                }

                                PathLine {
                                    x: 0
                                    y: 25
                                }

                                PathArc {
                                    x: 15
                                    y: 10
                                    radiusY: 15
                                    radiusX: 15
                                }

                                PathLine {
                                    x: popup.width/2 - 15
                                    y: 10
                                }

                                PathLine {
                                    x: popup.width/2
                                    y: 0
                                }
                                fillColor: "#808080"
                            }
                            antialiasing: true
                        }

                        Rectangle {
                            id: test
                            visible: false
                            color: "#ffffff"
                            anchors.fill: parent
                        }

                        OpacityMask {
                            opacity: 0.2
                            anchors.fill: parent
                            source: test
                            maskSource: shape
                        }
                    }
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

