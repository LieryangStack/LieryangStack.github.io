import QtQuick.Shapes
import QtQuick

/*
 * 为了让 Shape 输出任何可见的内容，Shape 必须包含至少一个 ShapePath
 */
Shape {
    width: 200
    height: 150
    anchors.centerIn: parent
    ShapePath {
        strokeColor: "pink"
        strokeWidth: 16
        fillColor: "black"
        /* 整条线段的开始和结尾是什么形状 */
        capStyle: ShapePath.RoundCap

        property int joinStyleIndex: 0

        property variant styles: [
            ShapePath.BevelJoin, /* 斜面形式的 */
            ShapePath.MiterJoin, /* 把线延长后，交叉到一个点 */
            ShapePath.RoundJoin /* 圆角 */
        ]

        joinStyle: styles[joinStyleIndex]

        /* 起始点 */ 
        startX: 30
        startY: 30
        /* 第一条线段终点（第二条线段起始点） */
        PathLine { x: 100; y: 100 }
        /* 第二条线段终点 */
        PathLine { x: 30; y: 100 }
    }
}