#include "myhello.h"
#include <QDebug>

MyHello::MyHello(QObject *parent)
    : QObject{parent}
{

}

MyHello::Color MyHello::color() const
{
    return m_color;
}

void MyHello::setColor(const MyHello::Color &color)
{
    if(color != m_color)
    {
        m_color = color;
        emit colorChanged();
    }
}

void MyHello::show()
{
    qDebug() << "MyHello::show() is called";
}

void MyHello::doSomething()
{
    qDebug() << "MyHello::doSomething() is called";
}
