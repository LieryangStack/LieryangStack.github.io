import QtQuick
import QtMultimedia

import QtQuick
import QtQuick.Window
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material 2.0

Window {
    id: window
    color: "transparent" //transparent
    flags: Qt.FramelessWindowHint | Qt.Window
    visible: true
    width: 840
    height: 500

    property point clickPos: Qt.point(0, 0);
    property int window_radius: 10 /* 窗口边角半径 */
    property int window_border_width: 2 /* 窗口边框宽度（这部分就是Window和Rectangle之间的间隔） */

    function handleMousePressed(mouse) {
        /* 如果处于全屏模式，直接返回 */
        if (window.visibility === Window.FullScreen)
            return;
        clickPos = Qt.point(mouse.x, mouse.y)
    }

    function handleMouseChanged(mouse) {
        /* 如果处于全屏模式，直接返回 */
        if (window.visibility === Window.FullScreen)
            return;
        /* 鼠标偏移量 */
        var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)

        /* 如果mainwindow继承自QWidget,用setPos */
        window.setX(window.x + delta.x)
        window.setY(window.y + delta.y)
    }

    /* 边框的阴影区域 */
    RectangularGlow {
        id: effect
        anchors.fill: rect
        glowRadius: 1
        spread: 0.8
        color: "gray"
        cornerRadius: rect.radius + glowRadius
    }

    /* 实际的窗口区域 */
    Rectangle {
        id: rect
        anchors.centerIn : parent
        width: parent.width - (2 * window_border_width)
        height: parent.height - (2 * window_border_width)
        radius: window_radius

        color : "#121212"

        gradient: Gradient {
          GradientStop { position: 0.0; color: "#ccfbff" }
          GradientStop { position: 1.0; color: "#ef96c5" }
        }

        /* 窗口顶部区域（此区域可以拖动窗口，双击放大窗口） */
        Rectangle {
            id: title_bar
            height: 50
            width: rect.width
            anchors.horizontalCenter: rect.horizontalCenter
            topLeftRadius: rect.radius
            topRightRadius: rect.radius

            color: "transparent"

            /* 为窗口添加鼠标事件 */
            MouseArea {

                id: mouseArea
                anchors.fill: parent
                anchors.margins: 10

                /* 只处理鼠标左击 */
                acceptedButtons: Qt.LeftButton

                onPressed: mouse => handleMousePressed(mouse)
                onPositionChanged: mouse => handleMouseChanged(mouse)
                onDoubleClicked: {
                    if (window.visibility === Window.FullScreen) {
                        window.visibility = Window.Windowed;
                        window.window_radius = 10;
                        window.window_border_width = 4;
                    } else {
                        window.visibility = Window.FullScreen;
                        window.window_radius = 0;
                        window.window_border_width = 0;
                    }
                }
            }
        }


        MouseArea {
            anchors.fill: parent
            /* 启用鼠标悬停检测 */
            hoverEnabled: true
            /* 用于动态改变鼠标指针的形状，基于鼠标位置来决定显示的光标类型 */
            cursorShape: {
              const p = Qt.point(mouseX, mouseY);
              const b = window_radius; // Increase the corner size slightly
              if (p.x < b && p.y < b) return Qt.SizeFDiagCursor; /* 对角线光标 */
              if (p.x >= width - b && p.y >= height - b) return Qt.SizeFDiagCursor;
              if (p.x >= width - b && p.y < b) return Qt.SizeBDiagCursor; /* 相反方向对角线光标 */
              if (p.x < b && p.y >= height - b) return Qt.SizeBDiagCursor;
              if (p.x < b || p.x >= width - b) return Qt.SizeHorCursor; /* 水平调整大小光标 */
              if (p.y < b || p.y >= height - b) return Qt.SizeVerCursor; /* 水平调整大小光标 */
            }
            acceptedButtons: Qt.NoButton // don't handle actual events
        }

        DragHandler {
            id: resizeHandler
            grabPermissions: TapHandler.TakeOverForbidden
            target: null
            onActiveChanged: if (active) {
                const p = resizeHandler.centroid.position;
                const b = window_radius + 100; // Increase the corner size slightly
                let e = 0;
                if (p.x < b) { e |= Qt.LeftEdge }
                if (p.x >= width - b) { e |= Qt.RightEdge }
                if (p.y < b) { e |= Qt.TopEdge }
                if (p.y >= height - b) { e |= Qt.BottomEdge }
                if (e !== 0)
                    window.startSystemResize(e);
            }
        }

        Rectangle {
            id: videoRect
            x: 200
            y: 100
            width: 580
            height: 270
            radius: 20
            color: "black"
            visible: false
        }

        MediaPlayer {
            id: player
            source: "sample.mp4"
            videoOutput: videoOutput
            autoPlay: true
            loops: -1
        }

        VideoOutput {
            id: videoOutput
            // fillMode: VideoOutput.PreserveAspectCrop
            anchors.fill: parent
            visible: false
        }

        OpacityMask {
            x: 200
            y: 100
            width: 580
            height: 270
            // anchors.fill: videoOutput
            source: videoOutput
            maskSource: videoRect
        }
     
    }
}


