import QtQuick
import QtQuick.Window
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material

Window {
    id: window
    visible: true
    width: 840
    height: 500

    Rectangle {
        id: rect
        width: 100; height: 100
        radius: 50
        color: "red"

        states: State {
            name: "moved"
            PropertyChanges { target: rect; x: 400; y: 400 }
        }

        transitions: Transition {
            /* 如果没有设定从那个状态到那个状态，就是所有状态都会执行 */
            to: "moved"
            PropertyAnimation { properties: "x,y"; easing.type: Easing.OutQuart; duration: 3000;}
        }

        MouseArea {
          anchors.fill: parent
          onClicked: {
            if (rect.state == "moved") 
              rect.state = ""
            else 
              rect.state = "moved"
          }
        }
    }


    // Rectangle {
    //   id: rect1
    //   width: 200
    //   height: 50
    //   color: "pink"
    //   y: rect.width

    //     Behavior on width {
    //       NumberAnimation { duration: 500 }
    //     }

    //     MouseArea {
    //         anchors.fill: parent
    //         onClicked: {
    //         if (rect1.width == 50)
    //           rect1.width = 200
    //         else
    //           rect1.width = 50
    //         }
    //     }
    // }

    // Rectangle {
    //   width: 100; height: 100
    //   anchors.top: rect1.bottom
    //   color: "blue"

    //   /* 按照顺序依次执行每个子动画 */
    //   SequentialAnimation on x {
    //       loops: Animation.Infinite
    //       PropertyAnimation { to: window.width - 100; duration: 2000}
    //       PropertyAnimation { to: 0; duration: 2000 }
    //   }
    // }

}