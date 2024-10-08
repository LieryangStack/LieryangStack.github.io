import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls.Material 2.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
    x: 500
    y: 300

    /* 窗口最大最小限定 */
    // maximumHeight: 480
    // maximumWidth: 640
    // minimumHeight: 300
    // minimumWidth: 400

    /* 窗口透明度设定 范围 0-1 */
    opacity: 1.0

    /* 自己定义的属性，QML都会自动生成相应的信号TestPropertyChanged */
    /* 自定义一个属性 */
    property int testProperty: 10

    property string buttonLabel: "btn1"

    /* 自定义一个信号 */
    signal testSignal

    /* 连接到信号， on+信号名（信号名第一个字母要大写） */
    onWidthChanged: {
        /* 打印输出到控制台 */
        console.log ("width: ", width);
    }
    onTestPropertyChanged: {
        console.log ("testProperty: ", testProperty);
    }
    onTestSignal: {

    }

    Button {
        id: btn1
        text: buttonLabel
    }

    Button {
        id: btn2
        text: "btn2"
        x: 100
        width: 50
        height: 50
        objectName: "btn2 "
        background: Rectangle {
            border.color: btn2.focus ? "blue" : "black"
        }
        onClicked: {
            console.log ("btn2 clicked");
        }
    }
}

