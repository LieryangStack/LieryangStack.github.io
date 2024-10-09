import QtQuick
import MyHello

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    MouseArea {
        anchors.fill: parent
        onClicked: {
            hello.begin();
            hello.doSomething()
            hello.show()
            /* 注意不是 hello.BLACK 因为枚举类型值属于类，而不是这个实例化的对象 */
            hello.color = MyHello.BLACK
        }
    }

    MyHello {
        id: hello
        /* 信号连接 */
        onBegin: {
            hello.doSomething()
        }

        onColorChanged: {
            console.log ("color changed")
        }
    }
}
