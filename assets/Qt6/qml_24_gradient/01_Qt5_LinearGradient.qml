import QtQuick
import Qt5Compat.GraphicalEffects

Item {
    width: 300
    height: 300

    LinearGradient {
        anchors.fill: parent
        start: Qt.point(0, 0)
        end: Qt.point(0, parent.height)
        gradient: Gradient {
			orientation: Gradient.Horizontal
            GradientStop { position: 0.0; color: "#FDAF42" } /* 黄橙 */
            GradientStop { position: 0.5; color: "#FC51FA" } /* 粉红火烈鸟 */
            GradientStop { position: 1.0; color: "#2CD4FF" } /* 蓝青色 */
        }
    }
}
