import QtQuick
import QtQuick.Window
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Controls.Material

Window {
    id: window
    visible: true
    width: 500
    height:800
    color: "pink"

    Image {
        id: image
        x: 100; y: 100
        width: 200
        height: 200
        source: "./image/city.jpg"
        fillMode: Image.PreserveAspectFit
        visible: false
    }

    /* 遮罩Rectangle */
    Rectangle {
        id: mask
        anchors.fill: image

        color:"white"
        Rectangle {
            anchors.centerIn: parent
            width: image.paintedWidth
            height: image.paintedHeight
            color:"black"
            radius: 10
        }
        visible: true //因为显示的是OpacityMask需要false
    }

    /* 或者使用item */
    Item {
        id: maskItem
        anchors.fill: image

        Rectangle {
            anchors.centerIn: parent
            width: image.paintedWidth
            height: image.paintedHeight
            color:"black"
            radius: 10
        }
        visible: false //因为显示的是OpacityMask需要false
    }

    // OpacityMask {
    //     anchors.fill: image
    //     source: image
    //     maskSource: mask
    // }

    MouseArea {
        id: dragArea
        anchors.fill: image
        drag.target: image  // 使矩形可拖动
        cursorShape: Qt.OpenHandCursor

        onPressed: {
            dragArea.cursorShape = Qt.ClosedHandCursor
        }
        onReleased: {
            dragArea.cursorShape = Qt.OpenHandCursor
        }
    }


}
