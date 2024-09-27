

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick 6.7
import QtQuick.Controls 6.7
import Loginnui1

Rectangle {
    id: rectangle
    width: 400
    height: 720

    color: Constants.backgroundColor
    clip: false

    Image {
        id: adventurePage
        anchors.fill: parent
        source: "images/adventurePage.jpg"
        fillMode: Image.Stretch

        Image {
            id: qt_logo_green_128x128px
            y: 34
            width: 96
            height: 72
            source: "images/qt_logo_green_128x128px.png"
            anchors.horizontalCenter: parent.horizontalCenter
            fillMode: Image.PreserveAspectFit
        }

        Text {
            id: tagLine
            y: 119
            color: "#eaeaea"
            text: qsTr("你准备好探索了吗？")
            font.pixelSize: 30
            anchors.horizontalCenterOffset: 1
            anchors.horizontalCenter: parent.horizontalCenter
            font.family: "Titillium Web ExtraLight"
        }

        EntryField {
            id: username
            y: 270
            text: qsTr("用户名或邮箱")
            anchors.horizontalCenter: parent.horizontalCenter
        }

        EntryField {
            id: password
            y: 340
            text: qsTr("密码")
            anchors.horizontalCenter: parent.horizontalCenter
        }

        PushButton {
            id: login
            y: 550
            text: qsTr("登录")
            anchors.horizontalCenter: parent.horizontalCenter
        }

        PushButton {
            id: createAccount
            y: 620
            text: "创建账号"
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
