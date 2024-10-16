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
		color: "transparent"

		GraphsView {
			id: graphsView
			anchors.fill: parent
		
			// theme: GraphsTheme {
			// 	backgroundVisible: false
			// 	plotAreaBackgroundColor: "#11000000"
			// }
			
			// layer.enabled: true
			// layer.effect: Glow { color: "red";  transparentBorder: true; radius: 85; spread:.8 }

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

			axisX: BarCategoryAxis {
				categories: ["2023", "2024", "2025"]
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

			SplineSeries {
				id: line
				// hoverable: true
				width: 2
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

		GraphsView {
			anchors.fill: parent
		
			// theme: GraphsTheme {
			// 	backgroundVisible: false
			// 	plotAreaBackgroundColor: "#11000000"
			// }
			
			// layer.enabled: true
			// layer.effect: Glow { color: "red";  transparentBorder: true; radius: 85; spread:.8 }

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

			axisX: BarCategoryAxis {
				categories: ["2023", "2024", "2025"]
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
                gridVisible: true
                subGridVisible: false
                labelsVisible: false
                visible: false
			}
		}

		Glow {
			anchors.fill: graphsView
			radius: 20
			spread: 1.1
			samples: radius * 2 + 1
			color: "white"
			source: graphsView
		}


	}
		
	
}