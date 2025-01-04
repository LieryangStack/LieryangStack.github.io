import QtQuick
import QtQuick.Window
import Qt5Compat.GraphicalEffects
import QtQuick.Studio.Effects

Item {
    id: root

    property int radius: 10 /* 窗口边角半径 */
    property int margins: 10 /* 窗口边框宽度（这部分就是Window和Rectangle之间的间隔） */
    property Item source_obj_background: linear
    property url imagePath: "image/city-1.png"

    required property Window windowRoot /* 该属性必须要被初始化，否则无法启动界面 */

    state: "NormalWindow"

    /* 边框的阴影区域 */
    RectangularGlow {
        id: rectRealeEffectGlow
        anchors.fill: rectReal
        glowRadius: 1
        spread: 0.8
        color: "gray"
        cornerRadius: root.radius + glowRadius
    }

    /* 实际的窗口区域 */
    Rectangle {
        id: rectReal
        radius: parent.radius
        anchors.fill: parent
        anchors.margins: root.margins

        /* 设定颜色背景 */
        color : "gray"

        /* 渐变色背景 */
        LinearGradient {
            id: linear
            visible: false
            anchors.fill: parent
            source: parent
            start: Qt.point(0, 0);
            end: Qt.point(0, parent.height)
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#F5F5F5" }
                GradientStop { position: 1.0; color: "#F5F5F5" }
                // GradientStop { position: 0.0; color: "#ccfbff" }
                // GradientStop { position: 1.0; color: "#ef96c5" }
            }
        }

        /* 图片背景 */
        Image {
            id: image
            visible: false
            anchors.fill: parent
            source: root.imagePath
        }

        /* 选择使用那个背景（渐变色、或者图片） */
        OpacityMask {
            anchors.fill: parent
            source: source_obj_background
            maskSource: parent
        }

        /* 窗口顶部区域（此区域可以拖动窗口，双击放大窗口） */
        Rectangle {
            id: title_bar
            height: 50
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            topLeftRadius: parent.radius
            topRightRadius: parent.radius

            color: "transparent"

            /* 移动窗口、双击放大缩小窗口 */
            MouseArea {

                property point clickPos: Qt.point(0, 0);

                id: mouseArea
                anchors.fill: parent
                anchors.margins: 5

                /* 只处理鼠标左击 */
                acceptedButtons: Qt.LeftButton

                /* 鼠标按压，记录此时鼠标的位置，便于接下来移动窗口 */
                onPressed: {
                    /* 如果处于全屏模式，直接返回 */
                    if (windowRoot.visibility === Window.FullScreen)
                        return;
                    clickPos = Qt.point(mouseX, mouseY)
                }

                /* 窗口移动 */
                onPositionChanged: {
                    /* 如果处于全屏模式，直接返回 */
                    if (windowRoot.visibility === Window.FullScreen)
                        return;
                    /* 鼠标偏移量 */
                    var delta = Qt.point(mouseX - clickPos.x, mouseY - clickPos.y)

                    /* 如果mainwindow继承自QWidget,用setPos */
                    windowRoot.setX(windowRoot.x + delta.x)
                    windowRoot.setY(windowRoot.y + delta.y)
                }

                /* 双击窗口 */
                onDoubleClicked: {
                    if (root.state !== "NormalWindow") {
                        root.state = "NormalWindow"
                    } else {
                        root.state = "FullWindow"
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
              const b = root.margins / 2;
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
                root.windowRoot.visibility: Window.FullScreen
                root.margins: 0
                root.radius: 0
            }
        },
        State {
            name: "MaximizedWindow"

            PropertyChanges {
                //root.windowRoot.visibility: Window.Maximized
                root.windowRoot.visibility: Window.FullScreen
                root.margins: 0
                root.radius: 0
            }
        },
        State {
            name: "NormalWindow"

            PropertyChanges {
                root.windowRoot.visibility: Window.Windowed
                root.margins: 10
                root.radius: 10
            }
        }
    ]
}

