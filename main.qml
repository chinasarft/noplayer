import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import OpenGLUnderQML 1.0
import QtQuick.Layouts 1.3
import "qrc:/ui/"

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
            id: icePlayer
        }
    }


    Rectangle{
        id: mytext
        anchors.top: player.bottom
        height: 32
        width: 96
        color: "#aaaaaa"
        Text{
            anchors.fill: parent
            color: "#f2f5f5"
            text: "contact"
            font.pointSize: 20
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    LineEdit {
        id: lineEdit
        anchors.top: player.bottom
        anchors.left: mytext.right
        //text: "placeholder"
        input.font.pointSize: 20
        input.color: "blue"
        width: parent.width - mytext.width
        input.height: 24

        Component.onCompleted: {
            console.log("xxxxxxxxxxxxxxLineEdit", width, parent.width, mytext.width)
        }
    }


    RowLayout{
        anchors.top: mytext.bottom
        width: parent.width
        height: 60
        spacing: 6


        Button {
            id: call
            Layout.fillWidth: true
            text: "call"

            onClicked: {
                call.color = "red"
                console.log(call.color)
            }

            background: Rectangle {
                implicitWidth: 100
                implicitHeight: 40
                color: call.down ? "#66ff53" : "#00ff00"
                border.color: "gray"
                border.width: 2
                radius: 4
            }
        }


        Button {
            id: hangup
            Layout.fillWidth: true
            text: "hangup"

            onClicked: {
                call.color = "red"
                console.log(call.color)
            }

            background: Rectangle {
                implicitWidth: 100
                implicitHeight: 40
                color: hangup.down ? "#ff4411" : "#ff0000"
                border.color: "gray"
                border.width: 2
                radius: 4
            }
        }
    }


    onClosing: {
        console.log("closesssssssssssss")
        icePlayer.Stop()
        Qt.quit()
    }

    Component.onCompleted: {
        console.log("yyyyyyyyyyyfrom qml")
    }

}
