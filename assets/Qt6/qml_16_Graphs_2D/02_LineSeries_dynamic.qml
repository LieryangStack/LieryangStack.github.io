import QtQuick
import QtGraphs

Window {
    width: 800; height: 500
    visible: true

    GraphsView {
        anchors.fill: parent
        anchors.leftMargin: -77
        anchors.rightMargin: -15
        anchors.bottomMargin: -40

        theme: GraphsTheme {
            backgroundVisible: false
            plotAreaBackgroundColor: "#11000000"
        }

        axisX: ValueAxis {
            lineVisible: false
            gridVisible: false
            subGridVisible: false
            labelsVisible: false
            visible: false
            id: xAxis
            max: 8
        }

        axisY: ValueAxis {
            lineVisible: false
            gridVisible: false
            subGridVisible: false
            labelsVisible: false
            visible: false
            tickInterval: 4
            id: yAxis
            max: 8
        }

        LineSeries {
            id: line
            property int divisions: 500
            property real amplitude: 0.5
            property real resolution: 0.5

            FrameAnimation {
                running: true

                onTriggered: {
                    for (let i = 0; i < line.divisions; ++i) {
                        let y = Math.sin(line.resolution*i)
                        y *= Math.cos(i)
                        y *= Math.sin(i / line.divisions * 3.2) * 3 * line.amplitude * Math.random()

                       line.replace(i, (i/line.divisions) * 8.0, y + 4)
                    }
                }
            }

            Component.onCompleted: {
                for (let i = 1; i <= divisions; ++i) {
                    append((i/divisions) * 8.0, 4.0)
                }
            }

            function change(newDivs) {
                let delta = newDivs - divisions

                if (delta < 0) {
                    delta = Math.abs(delta)
                    removeMultiple(count - 1 - delta, delta)
                } else {
                    for (let i = 0; i < delta; ++i) {
                        append(((count + i)/divisions) * 8.0, 4.0)
                    }
                }

                divisions = newDivs
            }
        }
    }

}
