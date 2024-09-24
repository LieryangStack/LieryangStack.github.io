import QtQuick
import QtMultimedia

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
        width: 200
        height: 200
        source: "./image/city.jpg"
        // fillMode: Image.PreserveAspectFit
        visible: false
    }

    /* 遮罩Rectangle */
    Rectangle {
        id: maskRec
        anchors.fill: image
        
        // color:"transparent"
        // Rectangle {
        //     anchors.centerIn: parent
        //     width: image.paintedWidth
        //     height: image.paintedHeight
        //     color:"black"
        //     radius: 10
        // }

        radius: 100
        visible: false
    }

    OpacityMask {
        id: mask
        anchors.fill: image
        source: image
        maskSource: maskRec
    }

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
