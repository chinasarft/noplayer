import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import OpenGLUnderQML 1.0

Window {
    visible: true
    width: 360
    height: 640
    color: "#000000"
    title: qsTr("Hello World")

    Rectangle{
        id: player
        color: "#00f5f5"
        objectName: "player"
        height: parent.height / 5 * 2
        width: parent.width
        IcePlayer {
        }
    }

    Text{
        id: mytext
        color: "#f2f5f5"
        anchors.top: player.bottom
        text: "Please select the best mobile os:"
    }

    Button {
        id: call
        text: "call"
        anchors.top: mytext.bottom
        //onClicked: {
        //    colorMaker.start()
        //}
    }

    Button {
        id: hangup
        text: "hangup"
        anchors.top: mytext.bottom
        anchors.left: call.right
        anchors.leftMargin: 5
    }

    TextInput {
        anchors.top: call.bottom
        anchors.left: parent.left
        text: "abcde"
        font.pointSize: 20
        color: "blue"
        focus: true

        Component.onCompleted: {
            console.log("xxxxxxxxxxxxxxfrom qml")
        }
    }

    Component.onCompleted: {
        console.log("yyyyyyyyyyyfrom qml")
    }

}
