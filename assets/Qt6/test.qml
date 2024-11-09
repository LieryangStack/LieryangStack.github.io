import QtQuick
import QtQuick.Controls.Basic

Window {
    id: rootWindow
    width: 640
    height: 480
    visible: true
    title: ""
    flags: Qt.Window |Qt.FramelessWindowHint
    color: "transparent"

    property bool isMinimized: false

    onVisibilityChanged: {
        if(rootWindow.visibility === Window.Minimized){
            rootWindow.isMinimized = true
            rootWindow.flags = Qt.Window
        } else if(rootWindow.isMinimized === true){
            rootWindow.flags = Qt.Window | Qt.FramelessWindowHint
            rootWindow.isMinimized = false
        }
    }

Rectangle {
        width: 600
        height: 440
        anchors.centerIn: parent

Row{
            Button {
                text: "Min"
                onClicked: () => rootWindow.showMinimized()
            }
        }
    }

}
