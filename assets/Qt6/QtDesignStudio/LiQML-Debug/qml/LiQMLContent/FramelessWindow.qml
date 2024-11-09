import QtQuick
import QtQuick.Window
import Qt5Compat.GraphicalEffects
import QtQuick.Studio.Effects

Item {
    id: itemRoot

    property int window_radius: 10 /* 窗口边角半径 */
    property int window_border_width: 5 /* 窗口边框宽度（这部分就是Window和Rectangle之间的间隔） */
    property Item source_obj_background: linear
    property url image_background_path: "image/city-1.png"
    property Window windowRoot

    state: "NormalWindow"

    /* 边框的阴影区域 */
    RectangularGlow {
        id: effect
        anchors.fill: rect1
        glowRadius: 1
        spread: 0.8
        color: "gray"
        cornerRadius: rect1.radius + glowRadius
    }

    /* 实际的窗口区域 */
    Rectangle {
        id: rect1
        anchors.centerIn : parent
        width: parent.width - (2 * window_border_width)
        height: parent.height - (2 * window_border_width)
        radius: window_radius

        /* 设定颜色背景 */
        color : "#121212"

        /* 渐变色背景 */
        LinearGradient {
            id: linear
            visible: false
            anchors.fill: parent
            start: Qt.point(0, 0);
            end: Qt.point(0, parent.height)
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#ccfbff" }
                GradientStop { position: 1.0; color: "#ef96c5" }
            }
        }

        /* 图片背景 */
        Image {
            id: image
            anchors.fill: parent
            source: itemRoot.image_background_path
            visible: false
        }

        /* 通过source选定可见背景类型 */
        OpacityMaskEffect {
            id: opacityMask
            visible: true
            anchors.fill: parent
            source: itemRoot.source_obj_background
            maskSource: rect1
        }

        /* 窗口顶部区域（此区域可以拖动窗口，双击放大窗口） */
        Rectangle {
            id: title_bar
            height: 50
            width: rect1.width
            anchors.horizontalCenter: rect1.horizontalCenter
            topLeftRadius: rect1.radius
            topRightRadius: rect1.radius

            color: "transparent"

            /* 移动窗口、双击放大缩小窗口 */
            MouseArea {

                property point clickPos: Qt.point(0, 0);

                id: mouseArea
                anchors.fill: parent
                anchors.margins: 10

                /* 只处理鼠标左击 */
                acceptedButtons: Qt.LeftButton

                /* 鼠标按压，记录此时鼠标的位置，便于接下来移动窗口 */
                onPressed: {
                    /* 如果处于全屏模式，直接返回 */
                    if (windowRoot.visibility === Window.FullScreen)
                        return;
                    clickPos = Qt.point(mouse.x, mouse.y)
                }

                /* 窗口移动 */
                onPositionChanged: {
                    /* 如果处于全屏模式，直接返回 */
                    if (windowRoot.visibility === Window.FullScreen)
                        return;
                    /* 鼠标偏移量 */
                    var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)

                    /* 如果mainwindow继承自QWidget,用setPos */
                    windowRoot.setX(windowRoot.x + delta.x)
                    windowRoot.setY(windowRoot.y + delta.y)
                }

                onDoubleClicked: {
                    if (itemRoot.state !== "NormalWindow") {
                        itemRoot.state = "NormalWindow"
                    } else {
                        itemRoot.state = "FullWindow"
                    }
                }
            }
        }

        /* 改变鼠标的形状到缩放样式，表示可以进行窗口缩放 */
        MouseArea {
            anchors.fill: parent
            /* 启用鼠标悬停检测 */
            hoverEnabled: true
            /* 用于动态改变鼠标指针的形状，基于鼠标位置来决定显示的光标类型 */
            cursorShape: {
              const p = Qt.point(mouseX, mouseY);
              const b = itemRoot.window_radius;
              if (p.x < b && p.y < b) return Qt.SizeFDiagCursor; /* 窗口左上角（对角线光标） */
              if (p.x < b && p.y >= height - b) return Qt.SizeBDiagCursor; /* 窗口左下角（对角线光标） */
              if (p.x >= width - b && p.y < b) return Qt.SizeBDiagCursor; /* 窗口右上角（对角线光标） */
              if (p.x >= width - b && p.y >= height - b) return Qt.SizeFDiagCursor; /* 窗口右下角（对角线光标） */
              if (p.x < b || p.x >= width - b) return Qt.SizeHorCursor; /* 水平方向调整大小光标 */
              if (p.y < b || p.y >= height - b) return Qt.SizeVerCursor; /* 竖直方向调整大小光标 */
            }
            acceptedButtons: Qt.NoButton
        }

        /* 窗口拉伸变化 */
        DragHandler {
            id: resizeHandler
            grabPermissions: PointerHandler.TakeOverForbidden
            target: parent
            onActiveChanged: if (active) {
                const p = resizeHandler.centroid.position;
                const b = 50;
                let e = 0;
                if (p.x < b) { e |= Qt.LeftEdge }
                if (p.x >= width - b) { e |= Qt.RightEdge }
                if (p.y < b) { e |= Qt.TopEdge }
                if (p.y >= height - b) { e |= Qt.BottomEdge }
                if (e !== 0)  {
                    windowRoot.startSystemResize(e);
                }
            }
        }
    }

    states: [
        State {
            name: "FullWindow"

            PropertyChanges {
                itemRoot.windowRoot.visibility: Window.FullScreen
                itemRoot.window_radius: 0
                itemRoot.window_border_width: 0
            }
        },
        State {
            name: "MaximizedWindow"

            PropertyChanges {
                itemRoot.windowRoot.visibility: Window.Maximized
                itemRoot.window_radius: 0
                itemRoot.window_border_width: 0
            }
        },
        State {
            name: "NormalWindow"

            PropertyChanges {
                itemRoot.windowRoot.visibility: Window.Windowed
                itemRoot.window_radius: 10
                itemRoot.window_border_width: 5
            }
        }
    ]
}

