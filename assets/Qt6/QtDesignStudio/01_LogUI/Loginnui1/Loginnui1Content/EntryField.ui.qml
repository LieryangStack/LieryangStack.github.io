

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick 2.15
import QtQuick.Controls 2.15

Button {
    id: control
    width: 270
    height: 46

    /* 组件的推荐大小 */
    implicitWidth: Math.max(
                       buttonBackground ? buttonBackground.implicitWidth : 0,
                       textItem.implicitWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(
                        buttonBackground ? buttonBackground.implicitHeight : 0,
                        textItem.implicitHeight + topPadding + bottomPadding)
    leftPadding: 4
    rightPadding: 4

    text: "My Button"
    hoverEnabled: false
    font.pointSize: 13
    font.family: "Titillium Web ExtraLight"


    /* background是Control类型的属性 （属性类型是Item）
     * 继承关系： Item -> Control -> AbstractButton -> Button
     */
    background: buttonBackground
    Rectangle {
        id: buttonBackground
        color: "#28e7e7e7"
        implicitWidth: 100
        implicitHeight: 40
        opacity: enabled ? 1 : 0.3
        radius: 23
        border.color: "#ffffff"
    }


    /* contentItem是Control类型的属性 （属性类型是Item）
     * 继承关系： Item -> Control -> AbstractButton -> Button
     */
    contentItem: textItem
    TextEdit {
        id: textItem
        text: control.text

        opacity: enabled ? 1.0 : 0.3
        color: "#ffffff"
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        font.family: "Titillium Web ExtraLight"
        font.pointSize: 13
        leftPadding: 30
    }

    states: [
        State {
            /* down是Button类型的属性，表示按钮是否按下 */
            name: "normal"
            /* State的when属性，这个属性成立的时候，该状态会被执行 */
            when: !control.down

            PropertyChanges {
                target: buttonBackground
                color: "#28e7e7e7"
            }

            PropertyChanges {
                target: textItem
                color: "#ffffff"
            }
        },
        State {
            name: "down"
            when: control.down
            PropertyChanges {
                target: textItem
                color: "#ffffff"
            }

            PropertyChanges {
                target: buttonBackground
                color: "#28e7e7e7"
            }
        }
    ]
}
