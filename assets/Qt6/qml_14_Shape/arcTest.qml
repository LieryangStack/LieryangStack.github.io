import QtQuick

Rectangle {
  width: 500; height: 500
  ArcItem {
    id: arc
    x: 10
    y: 10
    capStyle: 32
    /* 开始和结束的角度值 */
    begin: 0
    end: 270
    /* 线条宽度 */
    strokeWidth: 0
    /* 线条颜色 */
    strokeColor: "#000000"

    fillColor: "pink"

    outlineArc: true
  }
}
