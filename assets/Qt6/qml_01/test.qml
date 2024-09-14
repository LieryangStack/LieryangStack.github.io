import QtQuick 2.15
import QtGraphicalEffects 1.15  // 包含ShaderEffect

Rectangle {
    width: 400
    height: 200

    // 使用ShaderEffect为Text应用渐变效果
    ShaderEffect {
        width: parent.width
        height: parent.height

        // 定义输入图像为Text
        source: Text {
            text: "Hello, Gradient Text!"
            font.pixelSize: 40
            anchors.centerIn: parent
        }

        // 定义片段着色器来实现渐变效果
        fragmentShader: "
            uniform lowp sampler2D source;
            varying highp vec2 qt_TexCoord0;

            void main() {
                // 获取Text纹理的颜色
                lowp vec4 textColor = texture2D(source, qt_TexCoord0);
                
                // 定义渐变色 (蓝到绿)
                lowp vec4 gradientColor = mix(vec4(0.0, 0.5, 1.0, 1.0), vec4(0.0, 1.0, 0.0, 1.0), qt_TexCoord0.y);
                
                // 将渐变色应用到Text上
                gl_FragColor = textColor.a * gradientColor;
            }
        "
    }
}
