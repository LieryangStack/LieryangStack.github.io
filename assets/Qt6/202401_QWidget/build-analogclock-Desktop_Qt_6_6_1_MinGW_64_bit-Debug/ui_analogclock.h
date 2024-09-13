/********************************************************************************
** Form generated from reading UI file 'analogclock.ui'
**
** Created by: Qt User Interface Compiler version 6.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ANALOGCLOCK_H
#define UI_ANALOGCLOCK_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AnalogClock
{
public:
    QPushButton *pushButton;

    void setupUi(QWidget *AnalogClock)
    {
        if (AnalogClock->objectName().isEmpty())
            AnalogClock->setObjectName("AnalogClock");
        AnalogClock->resize(400, 400);
        pushButton = new QPushButton(AnalogClock);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(300, 0, 91, 31));

        retranslateUi(AnalogClock);
        QObject::connect(pushButton, &QPushButton::clicked, AnalogClock, qOverload<>(&QWidget::close));

        QMetaObject::connectSlotsByName(AnalogClock);
    } // setupUi

    void retranslateUi(QWidget *AnalogClock)
    {
        AnalogClock->setWindowTitle(QCoreApplication::translate("AnalogClock", "AnalogClock", nullptr));
        pushButton->setText(QCoreApplication::translate("AnalogClock", "\345\205\263\351\227\255", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AnalogClock: public Ui_AnalogClock {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ANALOGCLOCK_H
