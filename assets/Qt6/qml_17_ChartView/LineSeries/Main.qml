import QtQuick
import QtCharts

Window {
    id: root
    width: 800
    height: 500
    visible: true
    property int currentIndex: -1

    Rectangle {
        anchors.fill: parent
        ChartView {
            id: chartView
            title: "Driver Speeds, lap 1"
            anchors.fill: parent
            legend.alignment: Qt.AlignTop
            animationOptions: ChartView.SeriesAnimations
            antialiasing: true
        }

        SpeedsList {
            id: speedsList
            Component.onCompleted: {
                timer.start();
            }
        }

        Timer {
            id: timer
            interval: 300
            repeat: true
            triggeredOnStart: true
            running: false
            onTriggered: {
                root.currentIndex++;
                if (root.currentIndex < speedsList.count) {
                    // Check if there is a series for the data already
                    // (we are using driver name to identify series)

                    /* 我们会通过 driver 名称查看是否创建了该 LineSeries */
                    var lineSeries = chartView.series(speedsList.get(root.currentIndex).driver);
                    if (!lineSeries) {
                        lineSeries = chartView.createSeries(ChartView.SeriesTypeLine,
                                                            speedsList.get(root.currentIndex).driver);
                        chartView.axisY().min = 0;
                        chartView.axisY().max = 250;
                        chartView.axisY().tickCount = 6;
                        chartView.axisY().titleText = "speed (kph)";

                        chartView.axisX().titleText = "speed trap";
                        chartView.axisX().labelFormat = "%.0f";
                    }

                    /* 添加一个坐标点 */
                    lineSeries.append(speedsList.get(root.currentIndex).speedTrap,
                                      speedsList.get(root.currentIndex).speed);

                    /* 调整坐标轴 */
                    if (speedsList.get(root.currentIndex).speedTrap > 3) {
                        chartView.axisX().max = Number(speedsList.get(root.currentIndex).speedTrap) + 1;
                        chartView.axisX().min = chartView.axisX().max - 5;
                    } else {
                        chartView.axisX().max = 5;
                        chartView.axisX().min = 0;
                    }
                    chartView.axisX().tickCount = chartView.axisX().max - chartView.axisX().min + 1;
                } else {
                    // No more data, change x-axis range to show all the data
                    timer.stop();
                    chartView.animationOptions = ChartView.AllAnimations;
                    chartView.axisX().min = 0;
                    chartView.axisX().max = speedsList.get(root.currentIndex - 1).speedTrap;
                }
            }
        }

    }


}
