import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Window


Rectangle {
  id: page
  width: 600; height: 380
  color: "black"
  // rotation: 45 /* 矩形旋转 */

  /* 使用渐变色背景, 属性gradient只有水平方向或者垂直方向，如果想要指定点，就需要使用 LinearGradient */
  gradient: Gradient {
    GradientStop { position: 0.0; color: "#ccfbff" }
    GradientStop { position: 1.0; color: "#ef96c5" }
  }

  Text {
    id: helloText
    text: "Hello world!"
    
    anchors.horizontalCenter: page.horizontalCenter
    anchors.verticalCenter: page.verticalCenter
    
    font.pointSize: 50
    font.bold: true
  }

  LinearGradient {
    id: linerGradient
    anchors.fill: helloText
    source: helloText /* 通过设定source属性，可以对helloText区域填充渐变色 */
    
    start: Qt.point(0, 0)
    end: Qt.point(width, height)

    /* 使用渐变色背景，设定多个颜色 */
    gradient: Gradient {
      GradientStop { position: 0.0; color: "#2cd4ff" }
      GradientStop { position: 0.5; color: "#fc51fa" }
      GradientStop { position: 1.0; color: "#fdaf42" }
    }
  }
}
