

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick
import QtQuick.Controls
import Timeline
import QtQuick.Timeline 1.0

Rectangle {
    id: rectangle
    width: Constants.width
    height: Constants.height

    color: Constants.backgroundColor

    Button {
        id: button
        text: qsTr("Press me")
        anchors.verticalCenter: parent.verticalCenter
        checkable: true
        anchors.horizontalCenter: parent.horizontalCenter

        Connections {
            target: button
            onClicked: animation.start()
        }
    }

    Text {
        id: _text
        x: 298
        y: 74
        text: qsTr("测试动画效果")
        font.pixelSize: 12
        visible: false
    }

    Timeline {
        id: timeline
        animations: [
            TimelineAnimation {
                id: timelineAnimation
                running: false
                loops: 1
                duration: 1000
                to: 1000
                from: 0
            }
        ]
        startFrame: 0
        endFrame: 1000
        enabled: false
    }
    states: [
        State {
            name: "clicked"
            when: button.checked

            PropertyChanges {
                target: _text
                visible: true
                font.pixelSize: 20
            }

            PropertyChanges {
                target: timeline
                enabled: true
            }

            PropertyChanges {
                target: timelineAnimation
                running: true
            }
        }
    ]
}
