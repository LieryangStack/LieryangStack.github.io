import QtQuick
import QtGraphs

Window {
    width: 800; height: 500
    visible: true
    color: "#121212"

    GraphsView {
        anchors.fill: parent

        theme: GraphTheme {
            colorTheme: GraphTheme.ColorThemeDark
            gridMajorBarsColor: "#ccccff"
            gridMinorBarsColor: "#eeeeff"
            axisYMajorColor: "#ccccff"
            axisYMinorColor: "#eeeeff"
        }
        BarSeries {
            axisX: BarCategoryAxis {
                categories: ["2023", "2024", "2025"]
                lineVisible: false
            }
            axisY: ValueAxis {
                min: 0
                max: 10
                minorTickCount: 4
            }
            BarSet {
                values: [7, 6, 9]
            }
            BarSet {
                values: [9, 8, 6]
            }
        }
    }
}

