#include "analogclock.h"
#include <QStyleFactory>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("adwaita-dark"));
    AnalogClock w;
    w.show();
    return a.exec();
}
