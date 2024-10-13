
import QtQuick
import QtCharts

Window {
    id: root
    objectName: "root"
    width: 800
    height: 500
    visible: true
    
    
    ListModel {
        id: nameModel
        ListElement { name: "Alice"; city: "太原" }
        ListElement { name: "Bob"; city: "北京" }
        ListElement { name: "Jane"; city: "北京" }
        ListElement { name: "Harry"; city: "上海" }
        ListElement { name: "Wendy";  city: "天津" }
    }

    Component {
        id: nameDelegate
        Text {
			/* 这里需要的属性，是关联的 ListModel 中的， 如果是C++传入的model，StringList，属性应该是 modelData */
            required property string city
			required property string name
            text: city
            font.pixelSize: 24
        }
    }
	
    Component {     //instantiated when header is processed
        id: bannercomponent
        Rectangle {
            id: banner
            width: parent.width; height: 50
            gradient: clubcolors
            border {color: "#9EDDF2"; width: 2}
            Text {
                anchors.centerIn: parent
                text: "俱乐部成员"
                font.pixelSize: 32
            }
        }
    }

    Gradient {
        id: clubcolors
        GradientStop { position: 0.0; color: "#8EE2FE"}
        GradientStop { position: 0.66; color: "#7ED2EE"}
    }

	ListView {
        anchors.fill: parent
        clip: true
        model: nameModel
        delegate: nameDelegate
        /* 可装饰头 */
        header: bannercomponent
        /* 可装饰尾 */
        footer: Rectangle {
            width: parent.width; height: 30;
            gradient: clubcolors
        }
		
		/* 高亮 */
        highlight: Rectangle {
            width: parent.width
            color: "lightblue"
        }
		
		// section {
		// 	property: "city"
		// 	criteria: ViewSection.FullString
		// 	delegate: Rectangle {
		// 		color: "#b0dfb0"
		// 		width: parent.width
		// 		height: childrenRect.height + 4
		// 		Text {
		// 			anchors.horizontalCenter: parent.horizontalCenter
		// 			font.pixelSize: 16
		// 			font.bold: true
		// 			text: section
		// 		}
		// 	}
		// }
    }

}
