import QtQuick

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Rectangle {
        width: 200; height: 300
        Row {
            Image { id: img;
                    sourceSize { width: 100; height: 100 } source: "qrc:/content/qt-logo.png" }
            ShaderEffect {
                width: 200; height: 100
                property variant src: img
                vertexShader: "qrc:/qt/qml/shadereffects/content/shaders/myeffect.vert.qsb"
                fragmentShader: "qrc:/qt/qml/shadereffects/content/shaders/myeffect.frag.qsb"
            }
        }
    }

    Rectangle {
        x: 200; y: 200
        width: 200; height: 5
        color: "pink"
        radius: 20
        clip: true
        layer.enabled: true
        layer.effect: ShaderEffect {
            blending: true
            property variant src: img
            vertexShader: "qrc:/qt/qml/shadereffects/content/shaders/myeffect.vert.qsb"
            fragmentShader: "qrc:/qt/qml/shadereffects/content/shaders/myeffect.frag.qsb"
        }
    }
}
