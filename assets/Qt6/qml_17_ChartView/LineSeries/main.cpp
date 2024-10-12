#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QMainWindow>
#include <QQmlContext>
#include <QTimer>
#include <iostream>
#include <QQmlComponent>
#include <QFile>


int main(int argc, char *argv[])
{
    /* QtCharts 使用 Qt Graphics View Framework 进行绘制，因此 QApplication一定要被使用，而不是 QGuiApplication */
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    /* 如果 engine 在加载 QML 文件时遇到任何问题，例如 QML 文件中有语法错误，或者无法加载所需的组件，那么会发射 objectCreationFailed 信号。 */
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
    Qt::QueuedConnection);

    if (QFile::exists(":/UI.qml")) {
        qDebug() << "MyRect.qml exists in the resources.";
    } else {
        qDebug() << "MyRect.qml does not exist in the resources.";
    }

    engine.loadFromModule("LineSeries", "Main");
    // engine.load(QUrl(QStringLiteral("qrc:/UI.qml")));


    return app.exec();
}



// 折线图的形式显示数据
// QLineSeries *series = new QLineSeries();
// series->setPointsVisible(true); // 显示点
// series->setPointLabelsVisible(true); // 是否显示点坐标
// // 坐标x,y
// series->append(0, 6);
// series->append(2, 4);
// series->append(3, 8);
// series->append(7, 4);
// series->append(10, 5);
// *series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);
// // A line chart.
// qDebug() << series->type();


// QChart *chart = new QChart();
// // 返回图表中的图例对象
// chart->legend()->hide();
// // 将系列系列添加到图表中，并获得其所有权
// chart->addSeries(series);
// // 根据已添加到图表中的系列为图表创建轴
// chart->createDefaultAxes();
// chart->setTitle("简单折线图示例");

// QChartView *chartView = new QChartView(chart);
// chartView->setRenderHint(QPainter::Antialiasing);

// QMainWindow window;
// window.setWindowTitle("公众号：Qt历险记");
// window.setCentralWidget(chartView);
// window.resize(400, 300);
// window.show();

