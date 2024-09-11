#include "widget.h" /* 这里定义了 Widget 类 */
#include "ui_widget.h" /* 这是由UIC编译widget.ui后生成的文件 （这里定义的 Ui::Widget类) */


Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    this->ui->setupUi(this); /* 这里的ui就是Ui::Widget对象 */
}

Widget::~Widget()
{
    delete ui;
}

