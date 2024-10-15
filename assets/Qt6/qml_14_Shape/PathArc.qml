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
        preferredRendererType: Shape.CurveRenderer
        ShapePath {
            strokeWidth: 10
            strokeColor: "black"

            fillGradient: LinearGradient {
                x1: 20; y1: 20
                x2: 180; y2: 130
                GradientStop { position: 0; color: "blue" }
                GradientStop { position: 0.2; color: "green" }
                GradientStop { position: 0.4; color: "red" }
                GradientStop { position: 0.6; color: "yellow" }
                GradientStop { position: 1; color: "cyan" }
            }

            startX: 20; startY: 20

            PathArc {
                x: 20; y: 400
                radiusX: 200; radiusY: 200
                useLargeArc: true
            }
        }
    }
}
