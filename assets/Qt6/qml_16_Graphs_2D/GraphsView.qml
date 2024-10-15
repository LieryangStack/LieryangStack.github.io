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
		color: "gray"

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

			BarSeries {
				
				BarSet {
					values: [7, 6, 9]
				}
				BarSet {
					values: [9, 8, 6]
				}
			}
		}
	}
		
	
}