import QtQuick.Shapes
import QtQuick

Shape {
    width: 800
    height: 800
    anchors.centerIn: parent
    antialiasing: true
    smooth: true
    /* 决定渲染 */
    preferredRendererType: Shape.CurveRenderer
    ShapePath {
        strokeWidth: 5
        strokeColor: "black"
        startX: 20; startY: 20

        PathArc {
            x: 20; y: 400
            radiusX: 200; radiusY: 200
            useLargeArc: true
        }
    }
}