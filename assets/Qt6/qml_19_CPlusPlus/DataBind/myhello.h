#ifndef MYHELLO_H
#define MYHELLO_H

#include <QObject>

class MyHello : public QObject
{
    Q_OBJECT
    Q_ENUMS(Color)
    Q_PROPERTY(Color color READ color WRITE setColor NOTIFY colorChanged)
public:
    explicit MyHello(QObject *parent = nullptr);

    enum Color {
        RED,
        BLUE,
        BLACK
    };

    Color color() const;
    void setColor(const Color& color);

    /* 必须使用 Q_INVOKABLE */
    Q_INVOKABLE void show();

/* 必须是公有槽函数，才能在qml被调用（使用） */
public slots:
    void doSomething();

signals:
    void begin();
    void colorChanged();

private:
    Color m_color;
};

#endif // MYHELLO_H
