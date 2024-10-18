import QtQuick
import QtGraphs
import Qt5Compat.GraphicalEffects

Window {
    width: 800; height: 500
    visible: true
	color: "black"

	GraphsView {
        id: graphsView
        anchors.fill: parent

		Gradient {
            id: surfaceGradient
            GradientStop { position: 0.0; color: "darkgreen"}
            GradientStop { position: 0.15; color: "darkslategray" }
            GradientStop { position: 0.7; color: "peru" }
            GradientStop { position: 1.0; color: "white" }
        }

		theme: GraphsTheme {
			// colorScheme: GraphsTheme.ColorScheme.Light
            // theme: GraphsTheme.Theme.OrangeSeries
			plotAreaBackgroundColor: "transparent"
			backgroundVisible: false
			colorStyle: GraphsTheme.ColorStyle.ObjectGradient
			baseGradients: [surfaceGradient]
            // backgroundColor: "#101010"
            // backgroundVisible: true
            // grid.mainColor: Qt.rgba(0.2,0.2,0.2,1)
            // labelTextColor: "red"
            // labelBackgroundColor: "black"
            // labelFont.pointSize: 9
            // labelFont.family: "Roboto"
        }

		axisX: ValueAxis {
			max: 10
			subTickCount: 9
			lineVisible: false
			gridVisible: false
			subGridVisible: false
			labelsVisible: false
			visible: false
		}

		axisY: ValueAxis {
			max: 10
			subTickCount: 9
			lineVisible: false
			gridVisible: false
			subGridVisible: false
			labelsVisible: true
			visible: false
		}

		// layer.enabled: true
		// layer.effect: LinearGradient {
		// 	anchors.fill: parent
		// 	// start: Qt.point(0, 0)
		// 	// end: Qt.point(10, 10)
		// 	gradient: Gradient {
		// 		// orientation: Gradient.Horizontal
		// 		GradientStop { position: 0.0; color: "green" }
		// 		GradientStop { position: 1.0; color: "white" }
		// 	}
		// }

		AreaSeries {
			id: lowerArea
			color: "#FF00FF00"

			upperSeries: line

			/* 如果没有 lowerSeries，默认就是底部  */
			// lowerSeries: LineSeries {
			// 	XYPoint {x: 0; y: 0}
			// 	XYPoint {x: 10; y: 0}
			// }

			// brush: LinearGradient {
			// 	start: Qt.point(0, 0)
			// 	end: Qt.point(1, 1)
			// 	// 渐变的颜色，可以根据需要调整
			// 	GradientStop { position: 0.0; color: "lightblue" }
			// 	GradientStop { position: 1.0; color: "blue" }
			// }
		}

		LineSeries {
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
		visible: false
	}

	GraphsView {
        id: graphsView2
        anchors.fill: parent

		theme: GraphsTheme {
			// colorScheme: GraphsTheme.ColorScheme.Light
            // theme: GraphsTheme.Theme.OrangeSeries
			plotAreaBackgroundColor: "transparent"
			backgroundVisible: false
			colorStyle: GraphsTheme.ColorStyle.ObjectGradient
            // backgroundColor: "#101010"
            // backgroundVisible: true
            // grid.mainColor: Qt.rgba(0.2,0.2,0.2,1)
            // labelTextColor: "red"
            // labelBackgroundColor: "black"
            // labelFont.pointSize: 9
            // labelFont.family: "Roboto"
        }

		axisX: ValueAxis {
			max: 10
			subTickCount: 9
			lineVisible: false
			gridVisible: false
			subGridVisible: false
			labelsVisible: false
			visible: false
		}

		axisY: ValueAxis {
			max: 10
			subTickCount: 9
			lineVisible: false
			gridVisible: false
			subGridVisible: false
			labelsVisible: true
			visible: false
		}

		LineSeries {
			// hoverable: true
			width: 3
			color: "cyan"

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

	LinearGradient {
		anchors.fill: parent
		source: graphsView
		// start: Qt.point(0, 0)
		// end: Qt.point(10, 10)
		gradient: Gradient {
			// orientation: Gradient.Horizontal
			GradientStop { position: 0.0; color: "#4f00ffff" }
			GradientStop { position: 0.8; color: "#0000ffff" }
		}
	}

}