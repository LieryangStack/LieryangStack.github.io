import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Basic 2.15
import Qt5Compat.GraphicalEffects

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    color: "#f0f0f0"

    ComboBox {
        id: control
        model: ["First", "Second", "Third"]

        // 自定义委托，用于显示下拉框中的每一项
        delegate: ItemDelegate {
            id: delegate

            // 必需的属性，用于访问模型数据和索引
            required property var model
            required property int index

            width: control.width
            contentItem: Text {
                text: delegate.model[control.textRole]  // 显示模型中的文本
                color: "#21be2b"  // 文本颜色
                font: control.font  // 使用ComboBox的字体
                elide: Text.ElideRight  // 文本过长时省略号显示
                verticalAlignment: Text.AlignVCenter  // 垂直居中对齐
            }
            highlighted: control.highlightedIndex === index  // 高亮当前选中的项
        }

        // 自定义指示器，用于显示下拉箭头
        indicator: Canvas {
            id: canvas
            x: control.width - width - control.rightPadding  // 定位到右侧
            y: control.topPadding + (control.availableHeight - height) / 2  // 垂直居中
            width: 12
            height: 8
            contextType: "2d"

            // 连接ComboBox的pressedChanged信号，当按下状态改变时重新绘制指示器
            Connections {
                target: control
                function onPressedChanged() { canvas.requestPaint(); }
            }

            // 绘制指示器
            onPaint: {
                context.reset();
                context.moveTo(0, 0);
                context.lineTo(width, 0);
                context.lineTo(width / 2, height);
                context.closePath();
                context.fillStyle = control.pressed ? "#17a81a" : "#21be2b";  // 按下时改变颜色
                context.fill();
            }
        }

        // 自定义内容项，用于显示当前选中的文本
        contentItem: Text {
            leftPadding: 0
            rightPadding: control.indicator.width + control.spacing  // 右侧留出指示器的空间

            text: control.displayText  // 显示当前选中的文本
            font: control.font  // 使用ComboBox的字体
            color: control.pressed ? "#17a81a" : "#21be2b"  // 按下时改变颜色
            verticalAlignment: Text.AlignVCenter  // 垂直居中对齐
            elide: Text.ElideRight  // 文本过长时省略号显示
        }

        // 自定义背景，用于显示ComboBox的背景和边框
        background: Rectangle {
            implicitWidth: 120
            implicitHeight: 40
            border.color: control.pressed ? "#17a81a" : "#21be2b"  // 按下时改变边框颜色
            border.width: control.visualFocus ? 2 : 1  // 有焦点时加粗边框
            radius: 2  // 圆角半径
        }

        // 自定义弹出框，用于显示下拉列表
        popup: Popup {
            y: control.height - 1  // 定位到ComboBox下方
            width: control.width
            implicitHeight: contentItem.implicitHeight
            padding: 1

            // 弹出框的内容项，使用ListView显示下拉列表
            contentItem: ListView {
                clip: true  // 裁剪超出部分
                implicitHeight: contentHeight  // 根据内容高度调整高度
                model: control.popup.visible ? control.delegateModel : null  // 仅在弹出框可见时显示模型
                currentIndex: control.highlightedIndex  // 高亮当前选中的项

                // 添加垂直滚动条
                ScrollIndicator.vertical: ScrollIndicator { }
            }

            // 弹出框的背景
            background: Rectangle {
                color: "black"  // 设置背景矩形的颜色
                border.color: "red"  // 设置背景矩形的边框颜色
                border.width: 1  // 设置背景矩形的边框宽度
                radius: 4  // 设置背景矩形的圆角半径

                // 添加阴影效果
                layer.enabled: true  // 启用图层
                layer.effect: DropShadow {
                    horizontalOffset: 0  // 设置水平阴影偏移
                    verticalOffset: 2  // 设置垂直阴影偏移
                    radius: 8.0  // 设置阴影半径
                    samples: 16  // 设置阴影采样数
                    color: "#80000000"  // 设置阴影颜色
                }
            }
        }
    }
}

