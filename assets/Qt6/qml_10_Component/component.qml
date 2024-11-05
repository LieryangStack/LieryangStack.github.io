import QtQuick

Item {
  width: 400; height: 400

  Component {
    id: redSquare

    Rectangle {
        color: "red"
        width: 100
        height: 100
    }
  }

  Loader { sourceComponent: redSquare;
            Component.onCompleted: {
              console.log ("onCompleted",  width, height)
    }
  }

  Loader { sourceComponent: redSquare; x: 200 
            Component.onCompleted: {
            console.log ("onCompleted",  width, height)
    }
  }

  Component.onCompleted: {
    console.log ("onCompleted",  width, height)
  }
}



