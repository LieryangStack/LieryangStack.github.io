#include "mypushbutton.h"
#include <QPainter>

MyPushButton::MyPushButton(QWidget *parent)
    : QPushButton(parent)
{

}


void
MyPushButton::paintEvent(QPaintEvent *event) {
   // 在按钮上进行自定义绘制
   QPainter painter(this);

   // 设置笔和刷
   painter.setPen(Qt::black);  // 设置边框颜色
   painter.setBrush(Qt::NoBrush);  // 不填充

   // 在按钮中心绘制一个圆形
   painter.drawEllipse(rect().center(), 20, 20);  // 绘制一个半径为20的圆

   // 调用基类的 paintEvent，保留按钮的默认外观
   QPushButton::paintEvent(event);
}
