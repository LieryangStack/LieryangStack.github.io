

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick
import QtQuick.Controls.Material
import UntitledProject
import QtQuick.Controls 6.8

Rectangle {
    id: rectangle
    width: Constants.width
    height: Constants.height

    color: "#121212"
    // color: "white"
    Material.theme: Material.Dark
    Material.accent: Material.Cyan



        SplitView {
            id: idSplit
            anchors.fill: parent
            orientation: Qt.Horizontal //默认方向为水平方向

            //分裂器的组件，可以自定义，也可以使用默认
            handle: Rectangle {

                id: handleDelegate
                implicitWidth: 4
                implicitHeight: 4
                color: SplitHandle.pressed ? "#81e889" : (SplitHandle.hovered ? Qt.lighter("#c2f4c6", 1.1) : "#c2f4c6")
            }

            Rectangle {
                id: rec1
                color: "blue"

                //附加属性
                SplitView.minimumWidth: 50 //在SplitView中最小宽度
                SplitView.preferredWidth: 100 //item 默认显示宽度，范围在minimun和maximum之间，默认未设置，会使用item的implicitWidth(隐式宽度)
                SplitView.maximumWidth: 200 //在SplitView中最大宽度
            }

            //如果不指明implicitWidth 或者 minimumWidth/preferredWidth ,item可能会被其它的挤到最小
            Rectangle {
                id: rec2
                color: "green"

                SplitView.fillWidth: true //显示指明 占用剩余空间，如果不指定，默认最后可见的Item 占用 SplitView的剩余空间
            }

            Rectangle {
                id: rec3
                color: "red"

                SplitView.minimumWidth: 50
            }
        }


}
