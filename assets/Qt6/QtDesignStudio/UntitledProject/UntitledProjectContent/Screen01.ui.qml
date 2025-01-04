

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick
import QtQuick.Controls.Material
import UntitledProject
import QtQuick.Controls 6.8

Rectangle {
    id: rectangle
    width: Constants.width
    height: Constants.height

    // color: "#121212"
    color: "white"

    // Material.theme: Material.Dark

    // Material.accent: Material.Cyan
    Button {
        id: button
        x: 25
        y: 14
        text: qsTr("按钮")
    }

    CheckBox {
        id: checkBox
        x: 25
        y: 84
        text: qsTr("复选框")
    }

    ComboBox {
        id: comboBox
        x: 25
        y: 151
        width: 120
        height: 37
        flat: true
        editable: true
        model: ["First", "Second", "Third"]
    }

    Switch {
        id: _switch
        x: 34
        y: 240
        text: qsTr("Switch")
    }

    RoundButton {
        id: roundButton
        x: 89
        y: 284
        width: 79
        height: 77
        text: "测试"
    }

    Slider {
        id: slider
        x: 34
        y: 385
        value: 0.5
    }

    RadioButton {
        id: radioButton
        x: 358
        y: 284
        text: qsTr("Radio Button")
    }

    RadioButton {
        id: radioButton1
        x: 358
        y: 338
        text: qsTr("Radio Button")
    }

    SpinBox {
        id: spinBox
        x: 358
        y: 193
        width: 143
        height: 31
    }

    Tumbler {
        id: tumbler
        x: 534
        y: 84
        model: 10
    }

    ToolSeparator {
        id: toolSeparator
        x: 277
        y: 193
        width: 13
        height: 227
    }

    Label {
        id: label
        x: 218
        y: 41
        width: 298
        height: 42
        color: "#1c7ee1"
        text: qsTr("import QtQuick.Controls.FluentWinUI3")
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 15
    }
}
