import QtQuick

Rectangle {
    width: 600; height: 400
    color: "black"

    Rectangle {
        id: layered

        // opacity: 0
        color: "#220000FF"

        layer.enabled: true
        layer.effect: Rectangle {
          width: 1; height: 1
          color: "#223300FF"
        }

        width: 200
        height: 200

        Rectangle { width: 80; height: 80; color: "pink"; border.width: 1; }
        Rectangle { x: 20; y: 20; width: 80; height: 80; color: "red"; border.width: 1;}
    }

  //   Item {

  //     id: nonLayered

  //     opacity: 0.5

  //     width: 400
  //     height: 400

  //     Rectangle { width: 80; height: 80; border.width: 1 }
  //     Rectangle { x: 20; y: 20; width: 80; height: 80; border.width: 1 }
  // }

}