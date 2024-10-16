import QtQuick
import Qt5Compat.GraphicalEffects

Item {
	width: 300
	height: 300

	RadialGradient {
		anchors.fill: parent
		gradient: Gradient {
			GradientStop { position: 0.0; color: "white" }
			GradientStop { position: 0.5; color: "black" }
		}
	}
}
