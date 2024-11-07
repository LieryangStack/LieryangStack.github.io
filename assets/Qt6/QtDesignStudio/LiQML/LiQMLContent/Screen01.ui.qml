

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick
import QtQuick.Controls
import LiQML
import QtQuick.Studio.Effects

FramelessWindow {
    anchors.fill: parent

    Image {
        id: closeIcon
        x: 598
        width: 20
        height: 20
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 20
        anchors.topMargin: 20
        source: "image/close-icon.svg"
        layer.effect: ColorOverlayEffect {
            id: colorOverlay
            color: "#a6a0a0"
        }
        layer.enabled: true
        fillMode: Image.PreserveAspectFit

        MouseArea {
            id: mouseArea
            width: 100
            height: 100
            layer.enabled: true

            Connections {
                target: mouseArea
                onClicked: Qt.quit()
            }
        }
    }
}
