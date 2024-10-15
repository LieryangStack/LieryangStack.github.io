import QtQuick
import QtGraphs
import Qt5Compat.GraphicalEffects

Window {
    width: 800; height: 500
    visible: true

	GraphsView {
        id: chart
        anchors.fill: parent

		axisX: ValueAxis {
			max: 20
			subTickCount: 9
			lineVisible: false
			gridVisible: false
			subGridVisible: false
			labelsVisible: false
			visible: false
		}

		axisY: ValueAxis {
			max: 20
			subTickCount: 9
			lineVisible: false
			gridVisible: false
			subGridVisible: false
			labelsVisible: true
			visible: false
		}

		SplineSeries {
			id: line
			// hoverable: true
			width: 3
			color: "#00FF00"

			visible: true

			XYPoint {x: 0; y: 5}
			XYPoint {x: 1; y: 2}
			XYPoint {x: 2; y: 5}
			XYPoint {x: 3; y: 4}
			XYPoint {x: 4; y: 6}
			XYPoint {x: 5; y: 7}
			XYPoint {x: 6; y: 9}
			XYPoint {x: 7; y: 8}
			XYPoint {x: 8; y: 9}
			XYPoint {x: 9; y: 6}
			XYPoint {x: 10; y: 6}
			XYPoint {x: 11; y: 6}
			XYPoint {x: 12; y: 1}
			XYPoint {x: 13; y: 9}
			XYPoint {x: 14; y: 1}

			GraphTransition {
				GraphPointAnimation { duration: 1000; easingCurve.type: Easing.OutCubic  }
				SplineControlAnimation { duration: 1000; easingCurve.type: Easing.OutCubic }
			}
		}
	}

}