import QtQuick
import QtQuick.Controls
import QtQuick.Studio.Effects

Image {
    id: root
    source: "image/close.svg"

    property color buttonColor: "#a6a0a0"
    property alias cursorShape: mouseArea.cursorShape

    sourceSize.height: 32
    sourceSize.width: 32

    signal clicked
    signal entered
    signal exited

    layer.enabled: true
    layer.effect: ColorOverlayEffect {
        id: colorOverlay
        color: buttonColor
    }
    fillMode: Image.Stretch

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor

        onClicked: root.clicked()
        onEntered: root.entered()
        onExited: root.exited()
    }
}
