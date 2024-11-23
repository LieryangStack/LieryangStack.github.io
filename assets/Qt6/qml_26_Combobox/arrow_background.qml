background: Rectangle {
    id: rectangle
    color: "#00000000"
    anchors.fill: parent
    layer.enabled: true
    layer.effect: Glow {
        color: "#808080"
        radius: 8
        anchors.fill: parent
        spread: 0.1
        samples: 20
    }

    Shape {
        id: shape
        anchors.fill: parent
        smooth: true
        preferredRendererType: Shape.CurveRenderer
        
        ShaderEffectSource {
            id: windowSource
            visible: false
            anchors.fill: shape
            sourceRect: Qt.rect (rectangle.x + comboBox.x + popup.x, rectangle.x + comboBox.y + popup.y, shape.width, shape.height)
            sourceItem: screen01
        }

        Rectangle {
            id: blurRect
            visible: false
            anchors.fill: shape
            FastBlur {
                radius: 50
                anchors.fill: blurRect
                source: windowSource
            }
        }

        OpacityMask {
            anchors.fill: blurRect
            source: blurRect
            maskSource: parent
        }

        ShapePath {
            id: shapePath
            strokeWidth: 1
            strokeStyle: ShapePath.SolidLine
            strokeColor: "#00000000"

            startX: popup.width/2
            startY: 0

            PathLine { x: popup.width/2 + 15; y: 10}
            PathLine { x: popup.width - 15; y: 10}
            PathArc {x: popup.width; y: 25; radiusY: 15; radiusX: 15}
            PathLine {x: popup.width; y: popup.height - 15}
            PathArc {x: popup.width - 15; y: popup.height; radiusY: 15; radiusX: 15}

            PathLine {x: 15; y: popup.height}

            PathArc {x: 0; y: popup.height -15; radiusY: 15; radiusX: 15}

            PathLine {x: 0; y: 25}

            PathArc {x: 15; y: 10; radiusY: 15; radiusX: 15}

            PathLine {x: popup.width/2 - 15; y: 10}

            PathLine {x: popup.width/2; y: 0}

            fillColor: "#808080"
        }
        antialiasing: true
    }

    Rectangle {
        id: test
        visible: false
        color: "#ffffff"
        anchors.fill: parent
    }

    OpacityMask {
        opacity: 0.2
        anchors.fill: parent
        source: test
        maskSource: shape
    }
}