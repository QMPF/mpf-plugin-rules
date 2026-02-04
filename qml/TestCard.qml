import QtQuick

Item {
    Text {
        text: "Test Card"
        font.pixelSize: 20
        color: "white"
        anchors.centerIn: parent
    }

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.5
    }
}
