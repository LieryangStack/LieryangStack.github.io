import QtQuick
import QtQuick.Layouts
import QtGraphs

Item {
    id: mainView
    width: 1280
    height: 720

    RowLayout  {
        id: graphsRow

        readonly property real margin:  mainView.width * 0.02

        anchors.fill: parent
        anchors.margins: margin
        spacing: margin

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "#262626"
            border.color: "#4d4d4d"
            border.width: 1
            radius: graphsRow.margin
            // rotation: 90
            
            /* bargraph 柱状图 */
            GraphsView {
                anchors.fill: parent
                anchors.margins: 16
                theme: GraphTheme {
                    colorTheme: GraphTheme.ColorThemeDark
                }

                BarSeries {
                    axisX: BarCategoryAxis {
                        categories: [2024, 2025, 2026, 2027]
                        gridVisible: false
                        minorGridVisible: false
                    }
                    axisY: ValueAxis {
                        min: 20
                        max: 100
                        tickInterval: 10
                        minorTickCount: 9
                    }

                    BarSet {
                        values: [82, 50, 75, 30]
                        borderWidth: 2
                        color: "#373F26"
                        borderColor: "#DBEB00"
                    }
                }
            }
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "#262626"
            border.color: "#4d4d4d"
            border.width: 1
            radius: graphsRow.margin

            GraphsView {
                anchors.fill: parent
                anchors.margins: 16
                theme: GraphTheme {
                    readonly property color c1: "#DBEB00"
                    readonly property color c2: "#373F26"
                    readonly property color c3: Qt.lighter(c2, 1.5)
                    colorTheme: GraphTheme.ColorThemeDark
                    gridMajorBarsColor: c3
                    gridMinorBarsColor: c2
                    axisXMajorColor: c3
                    axisYMajorColor: c3
                    axisXMinorColor: c2
                    axisYMinorColor: c2
                    axisXLabelsColor: c1
                    axisYLabelsColor: c1
                }
                
                /* 折线的每个点，会被这个矩形标记 */
                component Marker : Rectangle {
                    width: 16
                    height: 16
                    color: "#ffffff"
                    radius: width * 0.5
                    border.width: 4
                    border.color: "#000000"
                }
                

                SeriesTheme {
                    id: seriesTheme
                    /* 折线一 和 折线二 的颜色 */
                    colors: ["#2CDE85", "#DBEB00"]
                }

                /* 折线一 */
                LineSeries {
                    id: lineSeries1
                    theme: seriesTheme
                    /* X轴刻度如何分配 */
                    axisX: ValueAxis {
                        max: 5
                        tickInterval: 1
                        minorTickCount: 9
                        labelDecimals: 1
                    }
                    /* Y轴刻度如何分配 */
                    axisY: ValueAxis {
                        max: 10
                        tickInterval: 1
                        minorTickCount: 4
                        labelDecimals: 1
                    }
                    width: 4
                    /* 标记每个点 */
                    pointMarker: Marker { }
                    XYPoint { x: 0; y: 0 }
                    XYPoint { x: 1; y: 2.1 }
                    XYPoint { x: 2; y: 3.3 }
                    XYPoint { x: 3; y: 2.1 }
                    XYPoint { x: 4; y: 4.9 }
                    XYPoint { x: 5; y: 3.0 }
                }
                
                /* 折线二 */
                LineSeries {
                    id: lineSeries2
                    theme: seriesTheme
                    width: 4
                    // pointMarker: Marker { }
                    XYPoint { x: 0; y: 5.0 }
                    XYPoint { x: 1; y: 3.3 }
                    XYPoint { x: 2; y: 7.1 }
                    XYPoint { x: 3; y: 7.5 }
                    XYPoint { x: 4; y: 6.1 }
                    XYPoint { x: 5; y: 3.2 }
                }
            }
        }
    }
}
