// AnimationExample.qml
 
import QtQuick
 
Image {
    id: root
    width: 480
    height: 640
    source: "image/background.png"
 
    property int padding: 40
    property int duration: 4000
    property bool running: true
 
    Image {
        id: box
        width: 200
        height: 200
        x: root.padding
        y: (root.height-height)/2
        source: "image/box_green.png"
 
        NumberAnimation on x {
            id: xAnimation
            from: root.padding
            to: root.width - box.width - root.padding
            // loops: Animation.Infinite /* 动画运行次数 */
            duration: root.duration
            running: root.running /* 控制动画运行状态 */
        }
        RotationAnimation on rotation {
            id: rotationAnimation
            from: 0
            to: 360
            // loops: Animation.Infinite
            duration: root.duration
            running: root.running
        }
    }
 
    MouseArea {
        id: myMouse
        anchors.fill: parent
        onClicked: {
            box.x = root.padding
            box.rotation = 0
            root.running = true
            // xAnimation.restart()
            // rotationAnimation.restart()
        }
    }
 
}