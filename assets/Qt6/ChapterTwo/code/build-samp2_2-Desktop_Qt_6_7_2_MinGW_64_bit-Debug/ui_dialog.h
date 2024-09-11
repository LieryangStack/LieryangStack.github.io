/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout_6;
    QRadioButton *radioBlack;
    QRadioButton *radioRed;
    QRadioButton *radioBlue;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_5;
    QCheckBox *checkBoxUnfer;
    QCheckBox *checkBoxItalic;
    QCheckBox *checkBoxBold;
    QPlainTextEdit *plainTextEdit;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnClear;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *btnOK;
    QPushButton *btnExit;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName("Dialog");
        Dialog->resize(800, 600);
        verticalLayout = new QVBoxLayout(Dialog);
        verticalLayout->setObjectName("verticalLayout");
        groupBox = new QGroupBox(Dialog);
        groupBox->setObjectName("groupBox");
        horizontalLayout_6 = new QHBoxLayout(groupBox);
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        radioBlack = new QRadioButton(groupBox);
        radioBlack->setObjectName("radioBlack");

        horizontalLayout_6->addWidget(radioBlack);

        radioRed = new QRadioButton(groupBox);
        radioRed->setObjectName("radioRed");

        horizontalLayout_6->addWidget(radioRed);

        radioBlue = new QRadioButton(groupBox);
        radioBlue->setObjectName("radioBlue");

        horizontalLayout_6->addWidget(radioBlue);


        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(Dialog);
        groupBox_2->setObjectName("groupBox_2");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy);
        groupBox_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        groupBox_2->setFlat(false);
        groupBox_2->setCheckable(false);
        groupBox_2->setChecked(false);
        horizontalLayout_5 = new QHBoxLayout(groupBox_2);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        checkBoxUnfer = new QCheckBox(groupBox_2);
        checkBoxUnfer->setObjectName("checkBoxUnfer");

        horizontalLayout_5->addWidget(checkBoxUnfer);

        checkBoxItalic = new QCheckBox(groupBox_2);
        checkBoxItalic->setObjectName("checkBoxItalic");

        horizontalLayout_5->addWidget(checkBoxItalic);

        checkBoxBold = new QCheckBox(groupBox_2);
        checkBoxBold->setObjectName("checkBoxBold");
        QPalette palette;
        QBrush brush(QColor(0, 255, 255, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush);
        QBrush brush1(QColor(33, 186, 200, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Text, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush);
        checkBoxBold->setPalette(palette);

        horizontalLayout_5->addWidget(checkBoxBold);


        verticalLayout->addWidget(groupBox_2);

        plainTextEdit = new QPlainTextEdit(Dialog);
        plainTextEdit->setObjectName("plainTextEdit");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(plainTextEdit->sizePolicy().hasHeightForWidth());
        plainTextEdit->setSizePolicy(sizePolicy1);
        plainTextEdit->setSizeIncrement(QSize(0, 0));
        plainTextEdit->setBaseSize(QSize(0, 0));
        QFont font;
        font.setPointSize(26);
        plainTextEdit->setFont(font);

        verticalLayout->addWidget(plainTextEdit);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        btnClear = new QPushButton(Dialog);
        btnClear->setObjectName("btnClear");

        horizontalLayout_4->addWidget(btnClear);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);

        btnOK = new QPushButton(Dialog);
        btnOK->setObjectName("btnOK");

        horizontalLayout_4->addWidget(btnOK);

        btnExit = new QPushButton(Dialog);
        btnExit->setObjectName("btnExit");
        btnExit->setBaseSize(QSize(0, 0));

        horizontalLayout_4->addWidget(btnExit);


        verticalLayout->addLayout(horizontalLayout_4);

        QWidget::setTabOrder(radioBlack, radioRed);
        QWidget::setTabOrder(radioRed, radioBlue);
        QWidget::setTabOrder(radioBlue, checkBoxUnfer);
        QWidget::setTabOrder(checkBoxUnfer, checkBoxItalic);
        QWidget::setTabOrder(checkBoxItalic, checkBoxBold);
        QWidget::setTabOrder(checkBoxBold, btnClear);
        QWidget::setTabOrder(btnClear, btnOK);
        QWidget::setTabOrder(btnOK, btnExit);
        QWidget::setTabOrder(btnExit, plainTextEdit);

        retranslateUi(Dialog);
        QObject::connect(btnExit, &QPushButton::clicked, Dialog, qOverload<>(&QDialog::close));
        QObject::connect(btnOK, &QPushButton::clicked, Dialog, qOverload<>(&QDialog::accept));

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QCoreApplication::translate("Dialog", "\344\277\241\345\217\267\344\270\216\346\247\275\347\232\204\344\275\277\347\224\250", nullptr));
        groupBox->setTitle(QCoreApplication::translate("Dialog", "\351\242\234\350\211\262", nullptr));
        radioBlack->setText(QCoreApplication::translate("Dialog", "\351\273\221\350\211\262", nullptr));
        radioRed->setText(QCoreApplication::translate("Dialog", "\347\272\242\350\211\262", nullptr));
        radioBlue->setText(QCoreApplication::translate("Dialog", "\350\223\235\350\211\262", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("Dialog", "\345\255\227\344\275\223", nullptr));
        checkBoxUnfer->setText(QCoreApplication::translate("Dialog", "\344\270\213\345\210\222\347\272\277", nullptr));
        checkBoxItalic->setText(QCoreApplication::translate("Dialog", "\346\226\234\344\275\223", nullptr));
        checkBoxBold->setText(QCoreApplication::translate("Dialog", "\345\212\240\347\262\227", nullptr));
        plainTextEdit->setPlainText(QCoreApplication::translate("Dialog", "Qt 6 C++\n"
"\345\274\200\345\217\221\346\214\207\345\215\227", nullptr));
        btnClear->setText(QCoreApplication::translate("Dialog", "\346\270\205\347\251\272", nullptr));
        btnOK->setText(QCoreApplication::translate("Dialog", "\347\241\256\345\256\232", nullptr));
        btnExit->setText(QCoreApplication::translate("Dialog", "\351\200\200\345\207\272", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
