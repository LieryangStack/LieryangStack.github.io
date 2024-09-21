 import QtQuick
 import Qt5Compat.GraphicalEffects

Item {
    width: 600
    height: 400

    Rectangle {
        id: background
        anchors.fill: parent
        color: "black"
    }

    RectangularGlow {
        id: effect
        anchors.fill: rect
        glowRadius: 50
        spread: 1
        color: "#5555ff"
        cornerRadius: rect.radius
    }

    Rectangle {
        id: rect
        color: "green"
        anchors.centerIn: parent
        width: Math.round(parent.width / 1.5)
        height: Math.round(parent.height / 2)
        radius: 25
    }


}
