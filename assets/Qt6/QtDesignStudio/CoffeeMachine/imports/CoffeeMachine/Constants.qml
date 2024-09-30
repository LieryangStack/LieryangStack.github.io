pragma Singleton
import QtQuick 2.15

QtObject {
    property FontLoader fontLoader: FontLoader {
        id: fontLoader
        source: "../../content/fonts/TitilliumWeb-Regular.ttf"
    }
    readonly property alias fontFamily: fontLoader.name

    readonly property int width: 1024
    readonly property int height: 768
    readonly property int leftSideBarWidth: 366

    readonly property color backgroundColor: "#443224"
    readonly property int defaultMargin: 8
}
