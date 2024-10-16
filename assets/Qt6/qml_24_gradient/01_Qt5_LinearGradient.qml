import QtQuick
import Qt5Compat.GraphicalEffects

Item {
    width: 300
    height: 300

    LinearGradient {
        anchors.fill: parent
        start: Qt.point(parent.width, parent.height)
        end: Qt.point(0, 0)
        gradient: Gradient {
			orientation: Gradient.Horizontal
            GradientStop { position: 0.0; color: "pink" }
            GradientStop { position: 1.0; color: "black" }
        }
    }
}
