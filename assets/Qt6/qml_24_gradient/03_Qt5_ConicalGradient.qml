import QtQuick
import Qt5Compat.GraphicalEffects

Item {
    width: 300
    height: 300

    ConicalGradient {
        anchors.fill: parent
        angle: 36.0
        verticalOffset: -100
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#A100FFFF" }
            GradientStop { position: 1.0; color: "#119CFDFF" }
        }
    }
}
