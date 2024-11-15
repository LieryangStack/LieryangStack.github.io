// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import LiQML
import QtQuick.Controls.Material
import Qt5Compat.GraphicalEffects
import QtQuick.Shapes

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

        ComboBox {
            id: comboBox
            x: 457
            y: 253
            width: 120

            // model: ListModel {
            //          id: model
            //          ListElement { text: "Banana" }
            //          ListElement { text: "Apple" }
            //          ListElement { text: "Coconut" }
            // }

            model: ["人体识别", "姿态识别", "3D分割1111111111111111111111", "郑智林想我了", "张三", "李四", "王五"]

            // 自定义委托，用于显示下拉框中的每一项
           delegate: ItemDelegate {
               id: delegate

               // 必需的属性，用于访问模型数据和索引
               required property var model
               required property int index

               // width: comboBox.width
               width: popup.width - 20
               contentItem: Text {
                   text: delegate.model[comboBox.textRole]  // 显示模型中的文本
                   color: "black"  // 文本颜色
                   font: comboBox.font  // 使用ComboBox的字体
                   elide: Text.ElideRight
                   horizontalAlignment: Text.AlignHCenter  // 文本过长时省略号显示
                   verticalAlignment: Text.AlignVCenter  // 垂直居中对齐
               }
               highlighted: comboBox.highlightedIndex === index  // 高亮当前选中的项
           }


            /* 自定义指示器，用于显示下拉箭头 */
            indicator: Image {
                id: arrow
                x: comboBox.width - arrow.width
                anchors.verticalCenter: parent.verticalCenter  /* 定位到ComboBox右侧 */
                anchors.right: parent.right
                anchors.rightMargin: 5  /* 偏移一点位置，留出一点空隙 */
                source: "image/arrow-down-svgrepo-com.svg"
                rotation: 0
                sourceSize.height: 20
                sourceSize.width: 25
            }

            /* 自定义内容项，用于显示当前选中的文本 */
            contentItem: Text {
                text: comboBox.displayText  // 显示当前选中的文本
                color: comboBox.pressed ? "#17a81a" : "black"  // 按下时改变颜色
                // verticalAlignment: Text.AlignVCenter
                font.bold: false
                elide: Text.ElideRight
                anchors.left: parent.left
                anchors.right: arrow.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.strikeout: false
                font.weight: Font.Normal
                font.pointSize: 12
            }

            /* 自定义背景，用于显示ComboBox的背景和边框 */
            background: Rectangle {
                color: "transparent"
                anchors.fill: parent
            }

            popup: Popup {
                id: popup
                y: arrow.height
                x: arrow.x + (arrow.width - width) / 2
                width: comboBox.width
                height: 200

                implicitHeight: listview1.contentHeight


                contentItem: ListView {
                    id: listview1
                    anchors.fill: parent
                    anchors.leftMargin: 5
                    anchors.rightMargin: 5
                    anchors.topMargin: 15
                    anchors.bottomMargin: 5
                    clip: true
                    model: comboBox.popup.visible ? comboBox.delegateModel : null
                    currentIndex: comboBox.highlightedIndex
                    ScrollIndicator.vertical: ScrollIndicator { }

                    delegate: Item {
                        width: ListView.view.width
                        height: 50

                        Text {
                            anchors.centerIn: parent
                            text: model.name
                        }

                        // 设置选中项的颜色
                        Rectangle {
                            color: "lightblue"   // 选中项的背景颜色
                            width: parent.width
                            height: parent.height
                            radius: 5             // 可选：设置圆角
                        }
                    }

                }

                background: Shape {
                    id: shape
                    anchors.fill: parent

                    antialiasing: true
                    smooth: true
                    /* 决定渲染 Shape.SoftwareRenderer  Shape.GeometryRenderer Shape.CurveRenderer */
                    preferredRendererType: Shape.CurveRenderer

                    //添加阴影效果
                    layer.enabled: true  // 启用图层
                    layer.effect: Glow {
                        radius: 3.0  // 设置阴影半径
                        spread: 0.2
                        samples: 16  // 设置阴影采样数
                        color: "gray"  // 设置阴影颜色
                    }

                    // 捕获窗口内容
                    ShaderEffectSource {
                        id: windowSource
                        sourceItem: screen01
                        anchors.fill: screen01
                        sourceRect: Qt.rect (x, y, width, height)
                        visible: true  // 不需要显示，直接捕捉背景内容
                    }

                    GaussianBlur {
                         anchors.fill: parent
                         source: windowSource
                         radius: 50
                         deviation: 500
                         samples: 100
                     }


                    ShapePath {
                        strokeWidth: 0.5
                        strokeColor: "transparent"

                        // fillGradient: LinearGradient {
                        //     x1: 0; y1: 0
                        //     x2: 0; y2: shape.height

                        //     GradientStop { position: 0.0; color: "#ccfbff" }
                        //     GradientStop { position: 1.0; color: "#ef96c5" }
                        // }

                        // strokeStyle: ShapePath.DashLine

                        fillColor: "white"

                        startX: popup.width/2; startY: 0

                        PathLine { x: popup.width/2 + 15; y: 10 }
                        PathLine { x: popup.width - 15; y: 10 }
                        PathArc  { x: popup.width; y:25; radiusX: 15; radiusY: 15}
                        PathLine { x: popup.width; y: popup.height - 15 }
                        PathArc  { x: popup.width - 15; y:popup.height; radiusX: 15; radiusY: 15}
                        PathLine { x: 15; y: popup.height }
                        PathArc  { x: 0; y: popup.height -15; radiusX: 15; radiusY: 15}
                        PathLine { x: 0; y: 25 }
                        PathArc  { x: 15; y:10; radiusX: 15; radiusY: 15}

                        PathLine { x: popup.width/2 - 15; y: 10 }
                        PathLine { x: popup.width/2; y: 0 }

                    }
                }
            }
        }

        Frame {
            id: frame
            x: 76
            y: 64
            width: 305
            height: 279

              RadioDelegate {
                  id: radioDelegate
                  x: 69
                  y: 155
                  text: qsTr("区域1")
              }

              RadioButton {
                  id: radioButton
                  x: 69
                  y: 109
                  text: qsTr("Radio Button")
              }

              Switch {
                  id: _switch
                  x: 78
                  y: 219
                  text: qsTr("Switch")
              }
        }

          SwitchDelegate {
              id: switchDelegate
              x: 151
              y: 359
              text: qsTr("Switch Delegate")
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

