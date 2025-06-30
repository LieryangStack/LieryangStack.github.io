import QtQuick
import QtQuick.Window
import QtQuick.Controls.Basic

Window {
  width: 640
  height: 480
  visible: true
  title: qsTr("hello World")

  Rectangle {

    width: 200; height: 200
    color: "red"

    clip: true

    Rectangle {

      width: 300; height: 300
      color: "green"
    }
    
  }
}

