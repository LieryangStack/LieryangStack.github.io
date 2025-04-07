

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/


import QtQuick
import QtQuick.Controls
import QtQuick.Studio.Effects
import LiQML

FramelessWindowForm {
    id: framelessWindow

    Row {
        id: row
        width: 152
        height: 50
        anchors.fill: framelessWindow.windowTitleBarContent
        anchors.margins: 10
        layoutDirection: Qt.RightToLeft
        spacing: 20

        FramelessWindowButton {
            id: windowCloseButton
            width: 19
            height: 19
            cursorShape: Qt.PointingHandCursor
            source: "image/li-window-close.svg"

            onClicked: Qt.quit()
            onEntered: buttonColor = "#5a5a5a"
            onExited: buttonColor = "#a6a0a0"
        }

        FramelessWindowButton {
            id: windowMaximizedButton
            width: 17
            height: 17
            cursorShape: Qt.PointingHandCursor
            source: "image/li-window-maximize.svg"


            onEntered: buttonColor = "#5a5a5a"
            onExited: buttonColor = "#a6a0a0"

            Connections {
                target: windowMaximizedButton
                onClicked: {
                   if (framelessWindow.state !== "NormalWindow") {
                       framelessWindow.state = "NormalWindow"
                       window.flags = Qt.FramelessWindowHint | Qt.Window
                   } else {
                       framelessWindow.state = "MaximizedWindow"
                   }
               }
            }
        }

        FramelessWindowButton {
            id: windowMinimizedButton
            width: 25
            height: 18
            cursorShape: Qt.PointingHandCursor
            source: "image/li-window-minimize.svg"

            onClicked: framelessWindow.windowRoot.visibility = Window.Minimized
            onEntered: buttonColor = "#5a5a5a"
            onExited: buttonColor = "#a6a0a0"
        }
    }
}
