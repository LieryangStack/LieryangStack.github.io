

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick
import QtQuick.Controls.Material
import UntitledProject

Rectangle {
    id: rectangle
    width: Constants.width
    height: Constants.height

    color: "white"

    Material.theme: Material.Light
    Material.accent: Material.Cyan
    Button {
        id: button
        x: 34
        y: 20
        text: qsTr("Button")
    }

    CheckBox {
        id: checkBox
        x: 34
        y: 95
        text: qsTr("Check Box")
    }

    ComboBox {
        id: comboBox
        x: 34
        y: 165
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
        width: 79
        height: 31
    }

    Tumbler {
        id: tumbler
        x: 534
        y: 84
        model: 10
    }
}
