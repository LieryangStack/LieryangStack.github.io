import QtQuick.Shapes
import QtQuick

Window {
    width: 800
    height: 800
    visible: true

    Shape {
        width: 800
        height: 800
        anchors.centerIn: parent
        antialiasing: true
        smooth: true
        /* 决定渲染 Shape.SoftwareRenderer  Shape.GeometryRenderer Shape.CurveRenderer */
        preferredRendererType: Shape.SoftwareRenderer
        ShapePath {
            strokeWidth: 10
            strokeColor: "black"
            startX: 20; startY: 20

            PathArc {
                x: 20; y: 400
                radiusX: 200; radiusY: 200
                useLargeArc: true
            }
        }
    }
}
