import QtQuick
import QtQuick.Window

Window {
  visible: true
  width: 640
  height: 480

  Rectangle {
    id: root
    color: "pink"
    width: 300
    height: 200
    state: "" /* 空字符串表示默认状态 */
    states: [
      State {
        name: "normal"
        PropertyChanges {target: root; color: "black"; width: 100; height: 50}
      },
      State {
        name: "red_color"
        PropertyChanges {target: root; color: "red"; width: 100; height: 80}
      },
      State {
        name: "blue_color"
        PropertyChanges {target: root; color: "blue"; width: 100; height: 250}
      }
    ]

    /* 鼠标事件 */
    MouseArea {
        anchors.fill: parent
        onPressed: {
          root.state = "red_color"
          console.log ("pressed")
        }
        onReleased: {
          root.state = "blue_color"
          console.log ("release")
        }
        onClicked: {
          root.state = "normal"
          console.log ("clicked") /* 按压和释放完成一次，是一次clicked */
        }
    }

    /* 不设置焦点 获取不了键盘事件 */
    focus: true
    /* 捕获键盘按键事件 */
    Keys.onPressed: (event)=> {
      console.log (event.key)
      if (event.key == Qt.Key_Space) {
        root.state = ""  /* 恢复到默认状态 */
      }
    }
  }
}