import QtQuick
import QtQuick.Controls


Window {
    id: root
    objectName: "root"
    width: 800
    height: 500
    visible: true

    ListView {
        id: listview
        width: 200; height: 320
        model: model1
        ScrollBar.vertical: ScrollBar { }

        delegate: Text {
            required property string type
            required property string size

            text: "Animal: " + type + ", " + size
        }
    }
}



