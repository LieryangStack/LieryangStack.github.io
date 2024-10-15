import QtQuick
import QtGraphs

Window {
    width: 800; height: 500
    visible: true
		
	GraphsView {
		anchors.fill: parent
	
		// theme: GraphsTheme {
		// 	backgroundVisible: false
		// 	plotAreaBackgroundColor: "#11000000"
		// }

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


		axisX: BarCategoryAxis {
			categories: ["2023", "2024", "2025"]
			lineVisible: false
			// visible: false
			// gridVisible: true
		}

		axisY: ValueAxis {
			min: 0
			max: 10
			// gridVisible: true
			// minorTickCount: 4
			// visible: false
		}

		// marginBottom: -40
		// marginLeft: -40
		// marginRight: 0
		// marginTop: 0

		shadowBarWidth: 2

		// shadowXOffset: -150
		// shadowYOffset: -150

		shadowColor: "gray"

		shadowVisible: true

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