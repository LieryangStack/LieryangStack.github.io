import QtQuick
import QtMultimedia
import QtQuick.Window
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Controls.Material

Window {
    id: window
    color: "transparent" //transparent
    flags: Qt.FramelessWindowHint | Qt.Window
    visible: true
    x: 50; y:50
    width: 1600
    height: 900

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

        // gradient: Gradient {
        //   GradientStop { position: 0.0; color: "#ccfbff" }
        //   GradientStop { position: 1.0; color: "#ef96c5" }
        // }

        // Image {
        //     id: image
        //     anchors.fill: parent
        //     source: "./image/city.jpg"
        // }

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

            RoundButton {
                width: 30
                height: 30
                x: parent.width - 60; y: 20
                // flat: true
                
                background: Rectangle {
                    width: 40
                    height: 40
                    anchors.centerIn: parent
                    radius: 30  // 圆角效果
                    border.color: "white"
                    border.width: 2
                    color: "white"
                    // visible: false
                    opacity: 0.5  // 控制透明度，使背景部分可见

                    FastBlur {
                        anchors.fill: parent
                        source: parent
                        radius: 32
                    }
                }

                icon.source: "close-icon.svg"
                icon.color: "gray"
                icon.height: 15
                icon.width: 15

                onPressed: window.close()
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
    }

    // 捕获窗口内容
    ShaderEffectSource {
        id: windowSource
        sourceItem: rect
        anchors.fill: rect
        sourceRect: Qt.rect (blurRect.x, blurRect.y, blurRect.width, blurRect.height)
        visible: false  // 不需要显示，直接捕捉背景内容
    }

    /* 边框的阴影区域 */
    RectangularGlow {
        id: effect2
        anchors.fill: blurRect
        glowRadius: 1
        spread: 0.1
        color: "gray"
        cornerRadius: blurRect.radius + glowRadius
    }

    /* 可移动的模糊区域 */
    Rectangle {
        id: blurRect
        
        x: 100; y: 100
        width: 300; height: 150
        radius: 20
        // anchors.fill: parent

        /* 模糊 */
       GaussianBlur {
            anchors.fill: blurRect
            source: windowSource
            radius: 50
            deviation: 500
            samples: 100
        }
        visible: false //因为显示的是OpacityMask需要false
    }

    /* 遮罩 */
    Rectangle {
        id: mask
        anchors.fill: blurRect
        radius: 20
        visible: false //因为显示的是OpacityMask需要false
    }

    OpacityMask {
        anchors.fill: blurRect
        source: blurRect
        maskSource: mask
    }

    /* 增加一层透明度为 0.2 的白色图层 */
    Rectangle {
        anchors.fill: mask
        color: "white"
        radius: 20
        opacity: 0.2
    }

    /* 增加边框 */
    // Rectangle {
    //     anchors.fill: mask
    //     border.color: "white"
    //     border.width: 1
    //     color: "transparent"
    //     radius: 20
    // }


    Text {
      anchors.fill: mask
      
      anchors.margins: 20
      
      text: "安装字体"
      color: "#4D4D4D"

      font.pointSize: 16
    }

    // 可移动的功能
    MouseArea {
        id: dragArea
        anchors.fill: blurRect
        drag.target: blurRect  // 使矩形可拖动
        cursorShape: Qt.OpenHandCursor

        onPressed: {
            dragArea.cursorShape = Qt.ClosedHandCursor
        }
        onReleased: {
            dragArea.cursorShape = Qt.OpenHandCursor
        }
    }
}

