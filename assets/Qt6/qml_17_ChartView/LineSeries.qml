import QtQuick
import QtQuick.Controls
import QtCharts

Rectangle {
    id: rectangle
    width: 500
    height: 400

    ChartView {
        id: spline
        x: 604
        y: 254
        width: 674
        height: 555
        SplineSeries {
            name: "SplineSeries"
            XYPoint {
                x: 0
                y: 1
            }

            XYPoint {
                x: 3
                y: 4.3
            }

            XYPoint {
                x: 5
                y: 3.1
            }

            XYPoint {
                x: 8
                y: 5.8
            }
        }
    }
    states: [
        State {
            name: "clicked"
        }
    ]
}
