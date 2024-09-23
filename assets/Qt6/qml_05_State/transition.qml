import QtQuick
import QtQuick.Window

Window {
  visible: true
  width: 640
  height: 480

  Rectangle {
    id: root
    state: "normal"
    states: [
      State {
        name: "normal"
        PropertyChanges {target: root; color: "black"; width: 50; height: 50}
      },
      State {
        name: "red_color"
        PropertyChanges {target: root; color: "red"; width: 100; height: 100}
      },
      State {
        name: "blue_color"
        PropertyChanges {target: root; color: "blue"; width: 200; height: 200}
      }
    ]

    /* 鼠标事件 */
    MouseArea {
        anchors.fill: parent
        onPressed: {
          if (root.state == "red_color")
            root.state = "normal"
          else
            root.state = "red_color"
        }
    }


    transitions: [
      Transition {
        from: "normal"
        to: "red_color"
        ColorAnimation { target: root; properties: "color"; duration: 500}
        PropertyAnimation {target: root; properties: "width,height"; duration: 500}
      },
      Transition {
        from: "red_color"
        to: "normal"
        ColorAnimation { target: root; properties: "color"; duration: 500}
        PropertyAnimation {target: root; properties: "width,height"; duration: 500}
      }
    ]

  }
}