#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv); /* 定义并创建应用程序对象 */
    Widget w; /* 定义并创建窗口对象 */
    w.show(); /* 显示窗口 */
    return a.exec(); /* 运行应用程序对象，开始应用程序的消息循环和事件处理 */
}
