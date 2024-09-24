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
        x: 100; y: 100
        id: image
        width: 200
        height: 200
        source: "./image/city.jpg"
        visible: false
    }

    /* 遮罩 */
    Item {
        id: mask
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

    OpacityMask {
        anchors.fill: image
        source: image
        maskSource: mask
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
