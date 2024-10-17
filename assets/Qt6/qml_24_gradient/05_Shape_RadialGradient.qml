import QtQuick
import QtQuick.Shapes


Shape {
    ShapePath {
        strokeWidth: 3
        strokeColor: "darkgray"
        
        fillGradient: RadialGradient {
            centerX: 150; centerY: 150
            centerRadius: 100
            focalX: centerX; focalY: centerY
            focalRadius: 50
            GradientStop { position: 0.0; color: "lightgreen" }
            GradientStop { position: 0.7; color: "yellow" }
            GradientStop { position: 1.0; color: "darkgreen" }
        }

        startX: 20; startY: 20

        PathLine { x: 400; y: 20}
        PathLine { x: 400; y: 400}
        PathLine { x: 20; y: 400}

    }
}