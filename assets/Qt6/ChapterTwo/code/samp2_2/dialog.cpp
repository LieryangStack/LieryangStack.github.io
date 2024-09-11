#include "dialog.h"
#include "ui_dialog.h"
#include <QPalette>

/* parent是NULL */
Dialog::Dialog(QWidget *parent) : QDialog(parent), ui(new Ui::Dialog)
{
    ui->setupUi(this);
    connect(ui->radioBlack, SIGNAL(clicked()), this, SLOT(do_setFontColor()));
    connect(ui->radioBlue, SIGNAL(clicked()), this, SLOT(do_setFontColor()));
    connect(ui->radioRed, SIGNAL(clicked()), this, SLOT(do_setFontColor()));
}

Dialog::~Dialog()
{
    delete ui;
}

/* 是否画下划线 */
void Dialog::on_checkBoxUnfer_clicked(bool checked)
{
    /* 返回的是 const QFont & 类型的引用，把这个引用内部的数据赋值给新的 font 对象 */
    QFont font = ui->plainTextEdit->font();
    /* 修改新对象font的下划线格式是否显示 */
    font.setUnderline(checked);
    /* 这里并不是把新创建的对象font设定为该Widget的字体，内部应该是进行了对象拷贝 */
    ui->plainTextEdit->setFont(font);
}


/* 斜体 */
void Dialog::on_checkBoxItalic_clicked(bool checked)
{
    QFont font = ui->plainTextEdit->font();
    font.setItalic(checked);
    ui->plainTextEdit->setFont(font);
}

/* 加粗 */
void Dialog::on_checkBoxBold_clicked(bool checked)
{
    QFont font = ui->plainTextEdit->font();
    font.setBold(checked);
    ui->plainTextEdit->setFont(font);
}

/* 清空内容 */
void Dialog::on_btnClear_clicked()
{
    ui->plainTextEdit->clear();
}

/* 设置文字颜色 */
void Dialog::do_setFontColor()
{
    QPalette plet = ui->plainTextEdit->palette();
    if (ui->radioBlue->isChecked())
        plet.setColor(QPalette::Text, Qt::blue);
    else if (ui->radioRed->isChecked())
        plet.setColor(QPalette::Text, Qt::red);
    else if (ui->radioBlack->isChecked())
        plet.setColor(QPalette::Text, Qt::black);
    else
        plet.setColor(QPalette::Text, Qt::black);

    ui->plainTextEdit->setPalette(plet);
}

