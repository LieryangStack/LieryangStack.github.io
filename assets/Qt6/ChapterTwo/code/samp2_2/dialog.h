#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class Dialog;
}
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_checkBoxUnfer_clicked(bool checked);

    void on_checkBoxItalic_clicked(bool checked);

    void on_checkBoxBold_clicked(bool checked);

    void on_btnClear_clicked();

    void do_setFontColor();

private:
    Ui::Dialog *ui;
};
#endif // DIALOG_H
