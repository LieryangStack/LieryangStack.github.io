import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls.Material 2.0

ApplicationWindow {
  width: 400
  height: 400
  visible: true

  Button {
    id: button
    text: "一个特别的按钮"
    background: Rectangle {
            implicitWidth: 100
            implicitHeight: 40
            color: button.down ? "#d6d6d6" : "#f6ffff"
            border.color: "#26282a"
            border.width: 1
            radius: 4
    }
  }
}