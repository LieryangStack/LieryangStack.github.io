import QtQuick

Window {
    id: root
    objectName: "root"
    width: 800
    height: 500
    visible: true

    Row {
        anchors.fill: parent

        ListView {
            width: 100
            height: 100
            model: model1 /* 使用 C++ 端传递的模型 */

            delegate: Rectangle {
                height: 25
                width: 100
                Text {
                    text: modelData
                }
            }
        }

        /* 分割线 */
        Rectangle {
            width: 2
            height:  parent.height  /* 设置分割线的高度 */
            color: "gray"  /* 设置分割线的颜色 */
        }

        ListView {
            width: 100
            height: 100
            model: stringListModel  /* 使用 C++ 端传递的模型 */

            delegate: Rectangle {
                height: 25
                width: 100
                Text {
                    text: model.display /* 关键 */
                }
            }
        }
    }
}
