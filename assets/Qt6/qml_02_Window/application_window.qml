import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Fusion

ApplicationWindow {
    id: window
    width: 620
    height: 460
    visible: true

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            Action { text: qsTr("&New...") }
            Action { text: qsTr("&Open...") }
            Action { text: qsTr("&Save") }
            Action { text: qsTr("Save &As...") }
            MenuSeparator { }
            Action { text: qsTr("&Quit") }
        }
        Menu {
            title: qsTr("&Edit")
            Action { text: qsTr("Cu&t") }
            Action { text: qsTr("&Copy") }
            Action { text: qsTr("&Paste") }
        }
        Menu {
            title: qsTr("&Help")
            Action { text: qsTr("&About") }
        }
    }

    header: ToolBar {
          RowLayout {
              anchors.fill: parent
              ToolButton {
                  text: qsTr("‹")
                  onClicked: stack.pop()
              }
              Label {
                  text: "Title"
                  elide: Label.ElideRight
                  horizontalAlignment: Qt.AlignHCenter
                  verticalAlignment: Qt.AlignVCenter
                  Layout.fillWidth: true
              }
              ToolButton {
                  text: qsTr("⋮")
                  onClicked: menu.open()
              }
          }
      }

    footer: TabBar {
        id: bar
        width: parent.width
        TabButton {
            text: qsTr("Home")
        }
        TabButton {
            text: qsTr("Discover")
        }
        TabButton {
            text: qsTr("Activity")
        }
    }

    StackLayout {
        width: parent.width
        currentIndex: bar.currentIndex
        Item {
            id: homeTab
            Button {
                id: button
                x: parent.x + 10
                text: "关闭窗口"
                background: Rectangle {
                        implicitWidth: 100
                        implicitHeight: 40
                        color: button.down ? "#d6d6d6" : "#f6ffff"
                        border.color: "#26282a"
                        border.width: 1
                        radius: 10
                }
                onClicked : {
                  window.close()
                }
            }

             Frame {
                x: parent.width - 200
                ColumnLayout {
                    anchors.fill: parent
                    CheckBox { text: qsTr("E-mail") }
                    CheckBox { text: qsTr("Calendar") }
                    CheckBox { text: qsTr("Contacts") }
                }
            }

        }
        Item {
            id: discoverTab
             ColumnLayout {
            Switch {
                text: qsTr("Wi-Fi")
            }
            Switch {
                text: qsTr("Bluetooth")
            }
        }

        }
        Item {
            id: activityTab
        }
    }
}