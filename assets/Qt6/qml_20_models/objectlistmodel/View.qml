import QtQuick
import QtQuick.Controls


Window {
    id: root
    objectName: "root"
    width: 800
    height: 500
    visible: true

    ListView {
        id: listview
        width: 200; height: 320
        model: model1
        ScrollBar.vertical: ScrollBar { }

        delegate: Rectangle {
            width: listview.width; height: 25

            required color /* 通过添加 required 要求现有属性，关联到model对象的color */
            required property string name /* 如果当前对象没有该属性，则需要需要定义一个该属性，使用required关联到model对象的name */

            Text { text: parent.name }
        }
    }
}

