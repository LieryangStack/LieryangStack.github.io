import QtQuick
import QtCharts


Window {
    id: root
    objectName: "root"
    width: 800
    height: 500
    visible: true
    property int currentIndex: -1

    /* 自己定义X轴和Y轴 */
    ValuesAxis {
        id: axisX
        objectName: "axisX"
        min: 0
        max: 10
        tickCount: 5
    }

    ValuesAxis {
        id: axisY
        objectName: "axisY"
        min: 0
        max: 10
    }

    ChartView {
        id: chartView
        objectName: "chartView"
        // title: "Driver Speeds, lap 1"
        anchors.fill: parent
        legend.alignment: Qt.AlignTop
        animationOptions: ChartView.SeriesAnimations
        antialiasing: true

        LineSeries {
            id: lineSeries
            objectName: "lineSeries"
            name: "LineSeries"
            axisX: axisX
            axisY: axisY
            XYPoint { x: 0; y: 0 }
            XYPoint { x: 1.1; y: 2.1 }
            XYPoint { x: 1.9; y: 3.3 }
            XYPoint { x: 2.1; y: 2.1 }
            XYPoint { x: 2.9; y: 4.9 }
            XYPoint { x: 3.4; y: 3.0 }
            XYPoint { x: 4.1; y: 3.3 }
       }
    }
}
