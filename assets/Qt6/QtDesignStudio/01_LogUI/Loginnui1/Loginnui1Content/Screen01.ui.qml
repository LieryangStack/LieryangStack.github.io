

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick 6.7
import QtQuick.Controls 6.7
import Loginnui1
import QtQuick.Timeline 1.0

Rectangle {
    id: rectangle
    width: 400
    height: 720

    color: Constants.backgroundColor
    state: "login"
    /* 设置默认状态 */
    clip: false

    Image {
        id: adventurePage
        anchors.fill: parent
        source: "images/adventurePage.jpg"
        fillMode: Image.Stretch

        Image {
            id: qt_logo_green_128x128px
            width: 96
            height: 72
            anchors.top: parent.top
            anchors.topMargin: 34
            source: "images/qt_logo_green_128x128px.png"
            anchors.horizontalCenter: parent.horizontalCenter
            fillMode: Image.PreserveAspectFit
        }

        Text {
            id: tagLine
            color: "#eaeaea"
            text: qsTr("你准备好探索了吗？")
            anchors.top: parent.top
            anchors.topMargin: 119
            font.pixelSize: 30
            anchors.horizontalCenterOffset: 1
            anchors.horizontalCenter: parent.horizontalCenter
            font.family: "Titillium Web ExtraLight"
        }

        EntryField {
            id: username
            width: 270
            height: 46
            text: qsTr("用户名或邮箱")
            anchors.top: tagLine.bottom
            anchors.topMargin: 40
            anchors.horizontalCenter: parent.horizontalCenter
        }

        EntryField {
            id: password
            width: 270
            height: 46
            text: qsTr("密码")
            anchors.top: username.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
        }

        EntryField {
            id: repeatPassword
            width: 270
            height: 46
            opacity: 0
            text: qsTr("确认密码")
            anchors.top: password.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Column {
            y: 578
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 35
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 20
            PushButton {
                id: login
                width: 270
                text: qsTr("登录")
            }

            PushButton {
                id: createAccount
                width: 270
                visible: true
                text: "创建账号"

                Connections {
                    target: createAccount
                    onClicked: rectangle.state = "createAccount"
                }
            }
        }
    }

    Timeline {
        id: timeline
        animations: [
            TimelineAnimation {
                id: toCreateAccountState
                running: false /* 动画运行状态 */
                loops: 1
                duration: 1000
                to: 1000
                from: 0
            }
        ]
        startFrame: 0
        endFrame: 1000
        enabled: false /* 时间轴使能 */

        KeyframeGroup {
            target: repeatPassword
            property: "opacity"
            Keyframe {
                value: 0
                frame: 0
            }

            Keyframe {
                value: 1
                frame: 1000
            }
        }

        KeyframeGroup {
            target: createAccount
            property: "opacity"
            Keyframe {
                value: 1
                frame: 0
            }

            Keyframe {
                value: 0
                frame: 1000
            }
        }

        KeyframeGroup {
            target: repeatPassword
            property: "anchors.topMargin"
            Keyframe {
                value: 20
                easing.bezierCurve: [0.392, 0.623, 0.565, 1, 1, 1]

                frame: 1000
            }

            Keyframe {
                value: 0
                frame: 0
            }
        }

        KeyframeGroup {
            target: repeatPassword
            property: "anchors"
        }
    }

    states: [
        State {
            name: "login"

            PropertyChanges {
                target: timeline
                enabled: false
            }

            PropertyChanges {
                target: toCreateAccountState
                running: false
            }
        },
        State {
            name: "createAccount"

            PropertyChanges {
                target: timeline
                enabled: true
            }

            PropertyChanges {
                target: toCreateAccountState
                running: true
            }
        }
    ]
}
