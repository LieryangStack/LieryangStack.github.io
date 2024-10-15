import QtQuick
import QtQuick.Window
import QtGraphs

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    GraphsView {
        anchors.fill: parent
        id: chart
        antialiasing: true

        // DateTimeAxis {
        //     id: axisX
        //     format: "HH:mm:ss"
        //     tickCount: 10 /* 刻度数 */
        // }

        axisX: ValueAxis {
            id: axisX
            min: 0
            max: 100
        }

        axisY: ValueAxis {
            id: axisY
            min: 0
            max: 100
        }

        // LineSeries{
        //     id: line
        //     color: "blue"
        // }

        SplineSeries {
            id: line
            width: 3
            color: "blue"
        }

        XYModelMapper {
            id: modelMapper
            model: lineModel // QStandardModel in C++
            series: line
            xSection: 0
            ySection: 2
            // firstRow: 0
            // xColumn: 0
            // yColumn: 2
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
