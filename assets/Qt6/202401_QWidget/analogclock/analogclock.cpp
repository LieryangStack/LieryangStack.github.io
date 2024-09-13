#include "analogclock.h"
#include "ui_analogclock.h"

#include <QPainter>
#include <QTime>
#include <QTimer>
#include <QPushButton>

#include "mypushbutton.h"

AnalogClock::AnalogClock(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AnalogClock)
{
    ui->setupUi(this);

    MyPushButton *button = new MyPushButton(this);
    button->setText("自定义按钮");
    button->resize(100, 50);
    connect (button, &QPushButton::clicked, this, &QWidget::close);

    /* 堆区创建一个定时器 */
    QTimer *timer = new QTimer(this);

    /* 定时器超时信号连接
     * QWidget::update() 槽函数重绘Widget区域，通常是会调用 paintEvent()函数
     */
    connect (timer, &QTimer::timeout, this, QOverload<>::of(&AnalogClock::update));
    // connect (timer,  &QTimer::timeout, [&]() {
    //     qDebug() << "Timer timeout";
    // });

    /* 启动定时器 */
    timer->start(1000);
}

AnalogClock::~AnalogClock()
{
    delete ui;
}

void
AnalogClock::paintEvent (QPaintEvent *) {
    static const QPoint hourHand[4] = {
        QPoint (5, 14),
        QPoint (-5, 14),
        QPoint (-4, -71),
        QPoint (4, -71)
    };

    const QColor hourColor(palette().color(QPalette::Text));

    int side = qMin(width(), height());

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(width() / 2, height() / 2);
    painter.scale(side / 200.0, side / 200.0);

    QTime time = QTime::currentTime();

    painter.setPen(Qt::NoPen);

    // 创建线性渐变，起点 (0, 0)，终点 (100, 100)
    QLinearGradient linearGrad(QPointF(5, 14), QPointF(-4, -71));

    // 设置颜色渐变的起始和终止点
    linearGrad.setColorAt(0, QColor("#CCFBFF"));  // 渐变起点颜色
    linearGrad.setColorAt(1, QColor("#EF96C5"));  // 渐变终点颜色

    painter.setBrush(linearGrad);

    painter.save();

    painter.rotate(30.0 * ((time.hour() + time.minute() / 60.0)));
    painter.drawConvexPolygon(hourHand, 4);
    painter.restore();
}
