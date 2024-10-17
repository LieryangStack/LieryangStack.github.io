import QtQuick
import QtQuick.Shapes


Shape {
    ShapePath {
        strokeWidth: 3
        strokeColor: "darkgray"
        
        fillGradient: LinearGradient {
            x1: 0; y1: 20
            x2: 0; y2: 400
            
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