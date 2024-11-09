

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

FramelessWindow {
    id: framelessWindow
    anchors.fill: parent

    Row {
        id: row
        width: 152
        height: 50
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 15
        anchors.topMargin: 15
        layoutDirection: Qt.RightToLeft
        spacing: 20

        Image {
            id: closeIcon
            width: 19
            height: 19
            source: "image/close.svg"
            property color mycolor: "#a6a0a0"
            sourceSize.height: 32
            sourceSize.width: 32

            layer.effect: ColorOverlayEffect {
                id: colorOverlay
                color: closeIcon.mycolor
            }
            layer.enabled: true
            fillMode: Image.PreserveAspectFit

            MouseArea {
                id: mouseArea
                anchors.fill: parent

                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                layer.enabled: true
                Connections {
                    target: mouseArea
                    onClicked: Qt.quit()
                }

                Connections {
                    target: mouseArea
                    onEntered: {
                        closeIcon.mycolor = "white"
                    }
                }

                Connections {
                    target: mouseArea
                    onExited: {
                        closeIcon.mycolor = "#a6a0a0"
                    }
                }
            }
        }

        Image {
            id: closeIcon1
            width: 17
            height: 17
            source: "image/maximize-svgrepo-com.svg"
            property color mycolor: "#a6a0a0"
            sourceSize.height: 32
            sourceSize.width: 32
            layer.enabled: true
            layer.effect: ColorOverlayEffect {
                id: colorOverlay1
                color: closeIcon1.mycolor
            }
            fillMode: Image.PreserveAspectFit
            MouseArea {
                id: mouseArea1
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                layer.enabled: true
                Connections {
                    target: mouseArea1
                    onClicked: {
                        if (framelessWindow.state !== "NormalWindow") {
                            framelessWindow.state = "NormalWindow"
                            window.color = "transparent"
                            window.flags = Qt.FramelessWindowHint | Qt.Window
                        } else {
                            framelessWindow.state = "MaximizedWindow"
                        }
                    }
                }
                Connections {
                    target: mouseArea1
                    onEntered: {
                        closeIcon1.mycolor = "white"
                    }
                }
                Connections {
                    target: mouseArea1
                    onExited: {
                        closeIcon1.mycolor = "#a6a0a0"
                    }
                }
            }
        }

        Image {
            id: closeIcon2
            width: 25
            height: 20
            source: "image/icons8-minimize.svg"
            property color mycolor: "#a6a0a0"
            sourceSize.height: 32
            sourceSize.width: 32
            layer.enabled: true
            layer.effect: ColorOverlayEffect {
                id: colorOverlay2
                color: closeIcon2.mycolor
            }
            fillMode: Image.Stretch
            MouseArea {
                id: mouseArea2
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                layer.enabled: true
                Connections {
                    target: mouseArea2
                    onClicked: framelessWindow.windowRoot.visibility = Window.Minimized
                }
                Connections {
                    target: mouseArea2
                    onEntered: {
                        closeIcon2.mycolor = "white"
                    }
                }
                Connections {
                    target: mouseArea2
                    onExited: {
                        closeIcon2.mycolor = "#a6a0a0"
                    }
                }
            }
        }
    }
}
