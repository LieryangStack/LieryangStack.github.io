import QtQuick
import QtQuick.Shapes

Rectangle {
    width: 600; height: 400
    color: hoverHandler.hovered ? "wheat" : "lightgray"
    containmentMask: shape

    /* HoverHandler 检测悬停的鼠标或平板笔触控笔光标
     * 通过绑定 hovered 属性是响应光标进入或离开父项的最简单方法
     */
    HoverHandler { id: hoverHandler }

    Shape {
        id: shape
        containsMode: Shape.FillContains

        ShapePath {
            fillColor: "lightsteelblue"
            /* 起点 */
            startX: 100; startY: 100
            PathArc {
                /* 终点 */
                x: 100; y: 160
                radiusX: 60; radiusY: 40
                useLargeArc: true
            }
            /* 上个路径的终点是这个线段的起点 */
            PathLine {
                /* 终点 */
                x: 100; y: 100
            }
        }
    }
}
