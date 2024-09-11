#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;  /* 一个名字空间 Ui，包含一个类 Widget */
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT /* 只有插入该宏，Widget类中才可以使用信号与槽、属性等功能 */

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
