import QtQuick
import Qt5Compat.GraphicalEffects

Item {
	width: 500
	height: 500

	RadialGradient {
		anchors.fill: parent
		angle: 45
		verticalOffset: 0
		horizontalOffset: -100

		verticalRadius: 300
		horizontalRadius: 100

		gradient: Gradient {
			GradientStop { position: 0.0; color: "red" }
			GradientStop { position: 0.5; color: "yellow" }
			GradientStop { position: 1.0; color: "blue" }
		}
	}
}
