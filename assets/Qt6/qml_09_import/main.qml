import QtQuick.Window
import QtQuick
import "VpfQuick"

VpfWindow {
    id: root

    Text {
        x: 50; y:50
        font.pixelSize: Style.textSize
        color: Style.textColor
        text: "Hello World"
    }
}

