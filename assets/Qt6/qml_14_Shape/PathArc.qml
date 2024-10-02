import QtQuick.Shapes
import QtQuick

Shape {
    width: 200
    height: 150
    anchors.centerIn: parent
    Path {
        startX: 100; startY: 0

        PathArc {
            x: 0; y: 100
            radiusX: 100; radiusY: 100
            useLargeArc: true
        }
    }
}