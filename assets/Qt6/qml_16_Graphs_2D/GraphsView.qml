import QtQuick
import QtGraphs
import Qt5Compat.GraphicalEffects


Window {
    width: 1280; height: 720
    visible: true

	Rectangle {
		anchors.fill: parent
		color : "#121212"
	}

	Rectangle {
		x:100; y: 100
		width: 800; height: 500
		radius: 20
		color: "white"


		// GraphsView {
		// 	anchors.fill: parent
		
		// 	theme: GraphsTheme {
		// 		colorScheme: GraphsTheme.ColorScheme.Automatic
		// 		// theme: GraphsTheme.Theme.OrangeSeries
		// 		plotAreaBackgroundColor: "transparent"
		// 		// colorStyle: GraphsTheme.ColorStyle.ObjectGradient
		// 		// baseGradients: [surfaceGradient]  /* 应该只有3D元素的时候起作用 */
		// 		// backgroundColor: "pink"
		// 		backgroundVisible: false
		// 		gridVisible: true
		// 	}

		// 	axisX: BarCategoryAxis {
		// 		categories: ["2023", "2024", "2025"]
        //         lineVisible: false
        //         gridVisible: false
        //         subGridVisible: false
        //         labelsVisible: false
        //         visible: false
		// 	}

		// 	axisY: ValueAxis {
		// 		min: 0
		// 		max: 10
		// 		tickInterval: 1.5
		// 		lineVisible: false
        //         gridVisible: true
        //         subGridVisible: false
        //         labelsVisible: false
        //         visible: false
		// 	}
		// }

		GraphsView {
			id: graphsView
			anchors.fill: parent
		
			// axisXSmoothing: 5
			// axisYSmoothing: 5

			theme: GraphsTheme {
				colorScheme: GraphsTheme.ColorScheme.Automatic
				// theme: GraphsTheme.Theme.OrangeSeries
				plotAreaBackgroundColor: "transparent"
				// colorStyle: GraphsTheme.ColorStyle.ObjectGradient
				// baseGradients: [surfaceGradient]  /* 应该只有3D元素的时候起作用 */
				// backgroundColor: "pink"
				backgroundVisible: false
				gridVisible: true
				// grid.mainColor: Qt.rgba(0.2,0.2,0.2,1)
				// labelTextColor: "red"
				// labelBackgroundColor: "black"
				// labelFont.pointSize: 9
				// labelFont.family: "Roboto"
			}

			// axisX: BarCategoryAxis {
			// 	categories: ["2023", "2024", "2025"]
			//           lineVisible: false
			//           gridVisible: false
			//           subGridVisible: false
			//           labelsVisible: false
			//           visible: false
			// }

			axisX: ValueAxis {
				min: 0
				max: 15
				lineVisible: false
                gridVisible: false
                subGridVisible: false
                labelsVisible: false
                visible: false
			}

			axisY: ValueAxis {
				min: 0
				max: 10
				lineVisible: false
                gridVisible: false
                subGridVisible: false
                labelsVisible: false
                visible: false
			}

			// marginBottom: -40
			// marginLeft: -40
			// marginRight: 0
			// marginTop: 0

			// shadowBarWidth: 2

			// shadowXOffset: -150
			// shadowYOffset: -150

			// shadowColor: "gray"

			// shadowVisible: true

			// BarSeries {
				
			// 	BarSet {
			// 		values: [7, 6, 9]
			// 	}
			// 	BarSet {
			// 		values: [9, 8, 6]
			// 	}
			// }

			layer.enabled: true

			/* 发光效果 */
			// layer.effect: Glow {
			// 	anchors.fill: graphsView
			// 	radius: 20
			// 	spread: 0.1
			// 	samples: radius * 2 + 1
			// 	color: "white"
			// 	source: graphsView
			// }

			/* 线段渐变 */
			layer.effect: LinearGradient {
				anchors.fill: parent
				start: Qt.point(0, 0)
				end: Qt.point(0, parent.height)
				gradient: Gradient {
					// orientation: Gradient.Horizontal
					GradientStop { position: 0.0; color: "#FC1CD5" }
					GradientStop { position: 0.5; color: "#1371EF" }
					GradientStop { position: 1.0; color: "#57EC99" }
				}
			}

			// layer.effect: RadialGradient {
			// 	anchors.fill: parent
			// 	verticalOffset: -parent.height / 2
			// 	gradient: Gradient {
			// 		GradientStop { position: 0.0; color: "white" }
			// 		GradientStop { position: 0.5; color: "purple" }
			// 	}
			// }

			// layer.effect: DropShadow {
			// 	anchors.fill: graphsView
			// 	// horizontalOffset: 3
			// 	verticalOffset: 10
			// 	radius: 50.0
			// 	samples: (radius *2) +1
			// 	color: "#4F00BFFF"
			// 	spread: 0.7
			// 	source: graphsView
			// }

			SplineSeries {
				id: line
				// hoverable: true
				width: 5
				color: "#00BFFF"

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

		// LinearGradient {
		// 	anchors.fill: graphsView
		// 	start: Qt.point(0, 0)
		// 	end: Qt.point(0, graphsView.height)
		// 	gradient: Gradient {
		// 		// orientation: Gradient.Horizontal
		// 		GradientStop { position: 0.0; color: "#FC1CD5" }
		// 		GradientStop { position: 0.5; color: "#1371EF" }
		// 		GradientStop { position: 1.0; color: "#57EC99" }
		// 	}
		// 	source: graphsView
		// }

		// Glow {
		// 	anchors.fill: graphsView
		// 	radius: 20
		// 	spread: 0.1
		// 	samples: radius * 2 + 1
		// 	color: "white"
		// 	source: graphsView
		// }
	}	
}