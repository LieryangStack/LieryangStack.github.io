import QtQuick
import QtQuick.Window
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material 2.0

Window {
  id: window
  color: "transparent"
  flags: Qt.FramelessWindowHint | Qt.Window
  visible: true
  width: 840
  height: 500

  property int bw: 3

  RectangularGlow {
    id: effect
    anchors.fill: rect
    glowRadius: 1
    spread: 1.2
    color: "green"
    cornerRadius: rect.radius + glowRadius
  }

  property point clickPos: Qt.point(0, 0);
  function handleMousePressed(mouse) {
    clickPos  = Qt.point(mouse.x, mouse.y)
  }

  function handleMouseChanged(mouse) {
    //鼠标偏移量
    var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)

    //如果mainwindow继承自QWidget,用setPos
    window.setX(window.x + delta.x)
    window.setY(window.y + delta.y)
  }

  Rectangle {
    id: rect
    anchors.fill: parent
    anchors.margins: 10
    radius: 10
    color : "#121212"
    gradient: Gradient {
      GradientStop { position: 0.0; color: "#ccfbff" }
      GradientStop { position: 1.0; color: "#ef96c5" }
    }

    
    MouseArea {
      anchors.fill: parent
      hoverEnabled: true
      cursorShape: {
          const p = Qt.point(mouseX, mouseY);
          const b = bw + 10; // Increase the corner size slightly
          if (p.x < b && p.y < b) return Qt.SizeFDiagCursor;
          if (p.x >= width - b && p.y >= height - b) return Qt.SizeFDiagCursor;
          if (p.x >= width - b && p.y < b) return Qt.SizeBDiagCursor;
          if (p.x < b && p.y >= height - b) return Qt.SizeBDiagCursor;
          if (p.x < b || p.x >= width - b) return Qt.SizeHorCursor;
          if (p.y < b || p.y >= height - b) return Qt.SizeVerCursor;
      }
      acceptedButtons: Qt.NoButton // don't handle actual events
    }

    DragHandler {
      id: resizeHandler
      grabPermissions: TapHandler.TakeOverForbidden
      target: null
      onActiveChanged: if (active) {
        const p = resizeHandler.centroid.position;
        const b = bw + 10; // Increase the corner size slightly
        let e = 0;
        if (p.x < b) { e |= Qt.LeftEdge }
        if (p.x >= width - b) { e |= Qt.RightEdge }
        if (p.y < b) { e |= Qt.TopEdge }
        if (p.y >= height - b) { e |= Qt.BottomEdge }
        window.startSystemResize(e);
      }
    }

    Rectangle {
      height: 20
      width: rect.width
      color: "transparent"

      /* 为窗口添加鼠标事件 */
      MouseArea {
        id: mouseArea
        anchors.fill: parent 
        /* 只处理鼠标左击 */
        acceptedButtons: Qt.LeftButton
        
        onPressed: mouse => handleMousePressed(mouse)
        onPositionChanged: mouse => handleMouseChanged(mouse)
        onDoubleClicked: {
          if (window.visibility === Window.FullScreen) {
              window.visibility = Window.Windowed;
          } else {
              window.visibility = Window.FullScreen;
          }
        }
      }

    }

    

    Button {
      id: button
      text: "关闭窗口"
      x : 200; y : 100
      background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40
        color: button.down ? "#d6d6d6" : "#f6ffff"
        border.color: "#ef96c5"
        border.width: 1
        radius: 10
      }
      onClicked: {
        window.close()
      }
    }
  }
}
