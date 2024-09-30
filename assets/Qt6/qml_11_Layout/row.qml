import QtQuick
import QtQuick.Window


Row {
    width: 800; height: 200
    spacing: 2
    Rectangle { color: "red"; width: 200; height: 200 }
    Rectangle { color: "green"; width: 80; height: 200 }
    Rectangle { color: "blue";  width: parent.width - 200; height: 80 }
}


