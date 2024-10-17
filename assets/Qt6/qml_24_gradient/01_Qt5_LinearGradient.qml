import QtQuick
import Qt5Compat.GraphicalEffects

Item {
    width: 500
    height: 500

    LinearGradient {
        anchors.fill: parent
        start: Qt.point(100, 100)
        end: Qt.point(0, height)
        gradient: Gradient {
			orientation: Gradient.Horizontal
            GradientStop { position: 0.0; color: "#FDAF42" } /* 黄橙 */
            GradientStop { position: 0.5; color: "#FC51FA" }
            GradientStop { position: 1.0; color: "#2CD4FF" } /* 青蓝 */
        }
    }

    Rectangle {
        x:100; y:100
        width:10; height: 10
        color: "red"
    }
}
