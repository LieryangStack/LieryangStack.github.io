// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import LiQML
import QtQuick.Controls.Material
import QtQuick.Controls 6.8
import QtQuick.Layouts


Window {
    id: window

    flags: Qt.FramelessWindowHint | Qt.Window

    visible: true
    color: "transparent"
    minimumWidth: 980
    minimumHeight: 780

    width: 980
    height: 780

    FramelessWindow {
        id: screen01
        anchors.fill: parent
        anchors.leftMargin: 0
        anchors.rightMargin: 0
        anchors.topMargin: 0
        anchors.bottomMargin: 0
        windowRoot: window

        Rectangle {
            id: windowContent
            color: "transparent"

            anchors.fill: parent.windowContent

            Row {
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.leftMargin: 20
                anchors.topMargin: 20
                spacing: 5
                Image {
                    id: icon
                    width: 36
                    height: 36
                    source: "image/icon.png"
                    fillMode: Image.PreserveAspectFit
                }

                Label {
                    id: label
                    width: 86
                    height: 36
                    color: "#a42e28"
                    text: qsTr("配置工具")
                    verticalAlignment: Text.AlignVCenter
                    styleColor: "#a42e28"
                    font.pointSize: 18
                    font.italic: false
                    font.bold: true
                }
            }

            Frame {
                id: frame
                x: 20
                y: 81
                width: 200
                height: 250
                background: Rectangle {
                    color :"#22a42e28"
                    radius: 10
                }

                ColumnLayout {
                    id: columnLayout
                    anchors.fill: parent

                    Row {
                        id: row1
                        x: 34
                        y: 74
                        width: 100
                        height: 100
                        spacing: 15

                        Label {
                            width: 50
                            height: 37
                            text: qsTr("串口号")
                            verticalAlignment: Text.AlignVCenter
                            font.pointSize: 12
                        }

                        ComboBox {
                            width: 110
                            height: 37
                            flat: true
                            editable: true
                            model: model1
                        }

                    }

                    Row {
                        x: 34
                        y: 74
                        width: 100
                        height: 100
                        spacing: 15

                        Label {
                            width: 50
                            height: 37
                            text: qsTr("波特率")
                            verticalAlignment: Text.AlignVCenter
                            font.pointSize: 12
                        }

                        ComboBox {
                            width: 110
                            height: 37
                            flat: true
                            editable: true
                            model: ["9600", "115200"]
                        }

                    }

                    Row {
                        x: 34
                        y: 74
                        width: 100
                        height: 100
                        spacing: 15

                        Label {
                            width: 50
                            height: 37
                            text: qsTr("数据位")
                            verticalAlignment: Text.AlignVCenter
                            font.pointSize: 12
                        }

                        ComboBox {
                            width: 110
                            height: 37
                            flat: true
                            editable: true
                            model: ["8"]
                        }

                    }

                    Row {
                        x: 34
                        y: 74
                        width: 100
                        height: 100
                        spacing: 15

                        Label {
                            width: 50
                            height: 37
                            text: qsTr("停止位")
                            verticalAlignment: Text.AlignVCenter
                            font.pointSize: 12
                        }

                        ComboBox {
                            width: 110
                            height: 37
                            flat: true
                            editable: true
                            model: ["1"]
                        }

                    }

                    Row {
                        x: 34
                        y: 74
                        width: 100
                        height: 100
                        spacing: 15

                        Label {
                            width: 50
                            height: 37
                            text: qsTr("校验位")
                            verticalAlignment: Text.AlignVCenter
                            font.pointSize: 12
                        }

                        ComboBox {
                            width: 110
                            height: 37
                            flat: true
                            editable: true
                            model: ["NONE"]
                        }
                    }
                }
            }

            ColumnLayout {
                x: 20
                y: 350
                height: 200
                Button {
                    id: button
                    height: 100
                    text: qsTr("打开串口")
                    font.pointSize: 12
                    font.bold: true
                    highlighted: true
                    Material.accent: Material.Red
                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 50
                }

                Button {
                    text: qsTr("下载配置")
                    font.pointSize: 12
                    font.bold: true
                    highlighted: true
                    Material.accent: Material.Red
                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 50
                }

                Button {
                    text: qsTr("读取配置")
                    font.pointSize: 12
                    font.bold: true
                    highlighted: true
                    Material.accent: Material.Red
                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 50
                }

                Button {
                    text: qsTr("重启设备")
                    font.pointSize: 12
                    font.bold: true
                    highlighted: true
                    Material.accent: Material.Red
                    Layout.preferredWidth: 200
                    Layout.preferredHeight: 50
                }
            }

            Label {
                id: label1
                x: 277
                y: 48
                width: 99
                height: 27
                text: qsTr("DTU设备参数")
                verticalAlignment: Text.AlignVCenter
                font.bold: true
                font.pointSize: 14
                color: "#a42e28"
            }


            Frame {
                x: 277
                y: 81
                width: 654
                height: 213
                background: Rectangle {
                    color :"#22a42e28"
                    radius: 10
                }

                ColumnLayout {
                    id: columnLayout1
                    anchors.fill: parent

                    RowLayout {

                        Label {
                            width: 120
                            text: "  IMEI"
                            font.pointSize: 12
                            Layout.preferredWidth: 135
                        }

                        Rectangle{
                            width: 250
                            height: 35
                            opacity: 1
                            color: "#00ffffff"
                            border.color: "#7b7979"
                            border.width: 1
                            radius: 4

                            Label {
                                text: ""
                                anchors.fill: parent
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                font.pointSize: 12
                                Layout.preferredWidth: 135
                            }


                        }




                    }

                    RowLayout {

                        Label {
                            width: 120
                            text: "  CAN通信波特率"
                            font.pointSize: 12
                            Layout.preferredWidth: 135
                        }


                        ComboBox {
                            model: ["5K","10K","20K","50K","100K"]
                            Layout.preferredHeight: 35
                        }

                    }


                    RowLayout {
                        x: 291
                        y: 75
                        spacing: 10
                        Switch {
                            text: qsTr("注册包")
                            font.pointSize: 12
                            Layout.preferredWidth: 130
                            Layout.preferredHeight: 41
                        }

                        ComboBox {
                            model: ["设备IMEI"]
                            Layout.preferredHeight: 35
                        }
                    }

                    RowLayout {
                        x: 291
                        y: 135
                        spacing: 10
                        Switch {
                            text: qsTr("心跳包")
                            font.pointSize: 12
                            Layout.preferredWidth: 130
                            Layout.preferredHeight: 41
                        }

                        ComboBox {
                            model: ["设备IMEI"]
                            Layout.preferredHeight: 35
                        }


                        Label {
                            text: "心跳周期"
                            font.pointSize: 12
                        }

                        Rectangle{
                            x: 10
                            y: 10
                            width: 100
                            height: 35
                            opacity: 1
                            color: "#00ffffff"
                            border.color: "#7b7979"
                            border.width: 1
                            radius: 4

                            TextInput {
                                text: backend.text
                                anchors.fill: parent
                                font.pixelSize: 12
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter

                                onTextChanged: backend.text = text
                            }
                        }

                        Label {
                            text: "ms"
                            font.pointSize: 12
                        }
                    }
                }

                Label {
                    x: -6
                    y: 207
                    width: 99
                    height: 27
                    text: qsTr("MQTT透传模式")
                    verticalAlignment: Text.AlignVCenter
                    font.bold: true
                    font.pointSize: 14
                    color: "#a42e28"
                }


                Frame {
                    x: -12
                    y: 247
                    width: 654
                    height: 240
                    background: Rectangle {
                        color :"#22a42e28"
                        radius: 10
                    }

                    ColumnLayout {
                        anchors.fill: parent

                        RowLayout {

                            Label {
                                width: 120
                                text: "IP/域名"
                                font.pointSize: 12
                                Layout.preferredWidth: 100
                            }


                            Rectangle{
                                width: 200
                                height: 35
                                opacity: 1
                                color: "#00ffffff"
                                border.color: "#7b7979"
                                border.width: 1
                                radius: 4

                                TextInput {
                                    text: qsTr("192.168.10.1")
                                    anchors.fill: parent
                                    font.pixelSize: 12
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }

                            Label {
                                x: 200
                                width: 120
                                text: "端口"
                                horizontalAlignment: Text.AlignHCenter
                                font.pointSize: 12
                                Layout.preferredWidth: 50
                            }


                            Rectangle{
                                width: 200
                                height: 35
                                opacity: 1
                                color: "#00ffffff"
                                border.color: "#7b7979"
                                border.width: 1
                                radius: 4

                                TextInput {
                                    text: qsTr("6002")
                                    anchors.fill: parent
                                    font.pixelSize: 12
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }

                        }

                        RowLayout {

                            Label {
                                width: 120
                                text: "用户名"
                                font.pointSize: 12
                                Layout.preferredWidth: 100
                            }


                            Rectangle{
                                width: 200
                                height: 35
                                opacity: 1
                                color: "#00ffffff"
                                border.color: "#7b7979"
                                border.width: 1
                                radius: 4

                                TextInput {
                                    text: qsTr("6002")
                                    anchors.fill: parent
                                    font.pixelSize: 12
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }

                            Label {
                                width: 120
                                text: "密码"
                                horizontalAlignment: Text.AlignHCenter
                                font.pointSize: 12
                                Layout.preferredWidth: 50
                            }


                            Rectangle{
                                width: 200
                                height: 35
                                opacity: 1
                                color: "#00ffffff"
                                border.color: "#7b7979"
                                border.width: 1
                                radius: 4

                                TextInput {
                                    text: qsTr("6002")
                                    anchors.fill: parent
                                    font.pixelSize: 12
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }

                        }

                        RowLayout {

                            Label {
                                width: 120
                                text: "客户端ID"
                                font.pointSize: 12
                                Layout.preferredWidth: 100
                            }


                            Rectangle{
                                width: 460
                                height: 35
                                opacity: 1
                                color: "#00ffffff"
                                border.color: "#7b7979"
                                border.width: 1
                                radius: 4

                                TextInput {
                                    text: qsTr("6002")
                                    anchors.fill: parent
                                    font.pixelSize: 12
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }

                        }

                        RowLayout {

                            Label {
                                width: 120
                                text: "订阅主题"
                                font.pointSize: 12
                                Layout.preferredWidth: 100
                            }


                            Rectangle{
                                width: 460
                                height: 35
                                opacity: 1
                                color: "#00ffffff"
                                border.color: "#7b7979"
                                border.width: 1
                                radius: 4

                                TextInput {
                                    text: qsTr("6002")
                                    anchors.fill: parent
                                    font.pixelSize: 12
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }

                        }

                        RowLayout {

                            Label {
                                width: 120
                                text: "发布主题"
                                font.pointSize: 12
                                Layout.preferredWidth: 100
                            }


                            Rectangle{
                                width: 460
                                height: 35
                                opacity: 1
                                color: "#00ffffff"
                                border.color: "#7b7979"
                                border.width: 1
                                radius: 4

                                TextInput {
                                    text: qsTr("6002")
                                    anchors.fill: parent
                                    font.pixelSize: 12
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }
                    }
                }


                Label {
                    x: -12
                    y: 500
                    width: 99
                    height: 27
                    text: qsTr("TCP透传模式")
                    verticalAlignment: Text.AlignVCenter
                    font.bold: true
                    font.pointSize: 14
                    color: "#a42e28"
                }
            }

            Switch {
                id: _switch
                x: 411
                y: 300
                text: qsTr("")
            }

            Switch {
                x: 418
                y: 587
                text: qsTr("")
            }

            Frame {
                x: 277
                y: 626
                width: 654
                height: 106
                background: Rectangle {
                    color :"#22a42e28"
                    radius: 10
                }

                ColumnLayout {
                    anchors.fill: parent

                    RowLayout {

                        Label {
                            width: 120
                            text: "IP/域名"
                            font.pointSize: 12
                            Layout.preferredWidth: 100
                        }


                        Rectangle{
                            width: 200
                            height: 35
                            opacity: 1
                            color: "#00ffffff"
                            border.color: "#7b7979"
                            border.width: 1
                            radius: 4

                            TextInput {
                                text: qsTr("192.168.10.1")
                                anchors.fill: parent
                                font.pixelSize: 12
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }

                        Label {
                            x: 200
                            width: 120
                            text: "端口"
                            horizontalAlignment: Text.AlignHCenter
                            font.pointSize: 12
                            Layout.preferredWidth: 50
                        }


                        Rectangle{
                            width: 200
                            height: 35
                            opacity: 1
                            color: "#00ffffff"
                            border.color: "#7b7979"
                            border.width: 1
                            radius: 4

                            TextInput {
                                text: qsTr("6002")
                                anchors.fill: parent
                                font.pixelSize: 12
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }

                    RowLayout {

                        Label {
                            width: 120
                            text: "波特率"
                            font.pointSize: 12
                            Layout.preferredWidth: 100
                        }

                        ComboBox {
                            model: ["9600","115200","20K","50K","100K"]
                            Layout.preferredHeight: 35
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

