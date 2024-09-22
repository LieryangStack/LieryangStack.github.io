import QtQuick
import QtQuick.Window
import Qt5Compat.GraphicalEffects
import QtQuick.Controls.Material

Window {
    id: window
    color: "black" //transparent
    // flags: Qt.FramelessWindowHint | Qt.Window
    visible: true
    width: 840
    height: 500

    // Rectangle {
    //     id: background
    //     anchors.fill: parent
    //     color: "transparent"
    // }

    RectangularGlow {
         id: effect
         anchors.fill: rect
         glowRadius: 1
         spread: 0.1
         color: "red"
         cornerRadius: rect.radius + glowRadius
     }


    Rectangle {
         id: rect
         width: parent.width / 1.2
         height: parent.height / 1.2
         color: "green"
         anchors.centerIn : parent
         radius: 20
     }




}
