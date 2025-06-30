import QtQuick

Window {
  width: 500; height: 600
  visible: true

  Item {
    id: nonlayered

    opacity: 0.5

    layer.enabled: false

    Rectangle { width: 80; 
                height: 80; 
                opacity: 0.5;
                color: "red"; 
                border.width: 1 }
    Rectangle { x: 20; 
                y: 20; 
                width: 80; 
                height: 80;
                opacity: 0.5;
                color: "green"; 
                border.width: 1 }
  }

  Item {
      id: layered
      x: 150
      width: 100; height: 100

      opacity: 0.5

      layer.enabled: true

      Rectangle { width: 80; 
                  height: 80; 
                  opacity: 0.5;
                  color: "red"; 
                  border.width: 1 }
      Rectangle { x: 20; 
                  y: 20; 
                  width: 80; 
                  height: 80;
                  opacity: 0.5;
                  color: "green"; 
                  border.width: 1 }
  }
    
  // Item {
  //     x: 300
  //     width: 100; height: 100

  //     opacity: 0.5

  //     layer.enabled: true

  //     Rectangle { width: 80; 
  //                 height: 80; 
  //                 color: "red"; 
  //                 border.width: 1 }
  //     Rectangle { x: 20; 
  //                 y: 20; 
  //                 width: 80; 
  //                 height: 80;
  //                 color: "green"; 
  //                 border.width: 1 }
  // }
  
}
