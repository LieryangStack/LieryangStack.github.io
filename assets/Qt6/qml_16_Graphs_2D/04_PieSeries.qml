import QtQuick
import QtGraphs

Window {
    width: 800; height: 500
    visible: true

	GraphsView {
        id: chart
        anchors.fill: parent

		PieSeries {
            id: pieOuter
            pieSize: 1
            holeSize: 0.8
            startAngle: -120
            endAngle: 120

            PieSlice { label: "Stall"; value: 1; color: "#ff0000"; labelVisible: true }
            PieSlice { label: "Optimal"; value: 4; color: "#00ff00"; labelVisible: false }
            PieSlice { label: "Overspeed"; value: 1; color: "#ffaa22"; labelVisible: false }
        }
	}

}