import QtQuick
import QtMultimedia

import QtQuick
import QtQuick.Window
import Qt5Compat.GraphicalEffects
import QtQuick.Controls
import QtQuick.Controls.Material 2.0

Item {
    width: 400
    height: 300

    // 第一个 sourceItem
    ShaderEffectSource {
        id: source1
        sourceItem: Image {
            source: "city.jpg"
            width: 400
            height: 300
        }
        smooth: true
    }

    // 第二个 sourceItem
    ShaderEffectSource {
        id: source2
        sourceItem: Image {
            source: "color.png"
            width: 400
            height: 300
        }
        smooth: true
    }

    ShaderEffect {
        anchors.fill: parent

        property var source1: source1
        property var source2: source2

        fragmentShader: "
            uniform sampler2D source1;
            uniform sampler2D source2;
            varying vec2 qt_TexCoord0;

            void main() {
                vec4 color1 = texture2D(source1, qt_TexCoord0);
                vec4 color2 = texture2D(source2, qt_TexCoord0);

                vec4 result = mix(color1, color2, 0.5);

                gl_FragColor = result;
            }
        "
    }
}
