import QtQuick
import QtQuick.Window
import QtQuick.Controls.Basic

Window {
  width: 640
  height: 480
  visible: true
  title: qsTr("hello World")

  Rectangle {
    width: 200; height: parent.height
    /* 启用clip，才能使得子项Flickable显示内容控制在Rectangle中，不超出显示 */
    clip: true

      Flickable {
        anchors.fill: parent
        // boundsBehavior: Flickable.StopAtBounds
        contentWidth: image.width; contentHeight: image.height

        Image { id: image; source: "image/city.jpg" }

        ScrollBar.vertical: ScrollBar {
          //  background: Rectangle {
          //     // color: "#ffffff"
          //   }
            // interactive: true
        }
        ScrollBar.horizontal: ScrollBar {
          //  background: Rectangle {
          //     color: "#ffffff"
          //   }
            // interactive: true
        }
    }
  }
}

