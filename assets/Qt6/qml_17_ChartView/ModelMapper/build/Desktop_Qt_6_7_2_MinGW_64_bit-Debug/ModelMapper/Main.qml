import QtQuick
import QtQuick.Window
import QtCharts

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    ChartView {
        anchors.fill: parent
        id: chart
        antialiasing: true

        // DateTimeAxis {
        //     id: axisX
        //     format: "HH:mm:ss"
        //     tickCount: 10 /* 刻度数 */
        // }

        ValueAxis {
            id: axisX
            min: 0
            max: 100
        }

        ValueAxis {
            id: axisY
            min: 0
            max: 100
        }

        // LineSeries{
        //     id: line
        //     axisX: axisX
        //     axisY: axisY
        // }

        SplineSeries {
            id: line
            axisX: axisX
            axisY: axisY
        }

        VXYModelMapper {
            id: modelMapper
            model: lineModel // QStandardModel in C++
            series: line
            firstRow: 0
            xColumn: 0
            yColumn: 2
        }

       //  Timer {
       //     interval: 1000 // 每秒钟检查一次数据变化
       //     running: true
       //     repeat: true
       //     onTriggered: {
       //         axisX.max = lineModel.rowCount() - 1
       //     }
       // }
    }
}
