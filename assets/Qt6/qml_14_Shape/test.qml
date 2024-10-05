import QtQuick
import Qt5Compat.GraphicalEffects

Rectangle {
    width: 300
    height: 300
    color: "#262626"
    Item {
        x: 0
        y: 0
        width: 300
        height: 300
     
        ArcItem {
            id: glowingArc
            x: 20
            y: 20
            width: 255
            height: 255
            opacity: 1
            dashPattern: [0.2, 0.3, 0.1, 0.2]
            strokeStyle: 1
            arcWidth: 0
            clip: true
            dashOffset: 2
            end: 0
            begin: 360
            strokeWidth: 13
            outlineArc: false
            capStyle: 0
            fillColor: "#00000000"
            strokeColor: "#b3cc9e00"
            visible: false
        }
        Glow {
            x: 2
            y: 2
            anchors.fill: glowingArc
            radius: 19
            samples: 17
            color: "#dbbb1f"
            source: glowingArc
            spread: -0.4
        }
        ConicalGradient {
            id: gradient
            anchors.fill: glowingArc
            angle: -360
            gradient: Gradient {
                GradientStop {
                    position: 0.1
                    color: "black"
                }
                GradientStop {
                    position: 0.0
                    color: "transparent"
                }
            }
            visible: false
        }
        OpacityMask {
            anchors.fill: gradient
            source: glowingArc
            maskSource: gradient
            invert: false
        }
    }
}
