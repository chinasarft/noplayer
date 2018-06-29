import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import OpenGLUnderQML 1.0
import QtQuick.Layouts 1.3
import "qrc:/ui/"

Window {
    id: window
    visible: true
    width: 360
    height: 640
    color: "#000000"
    title: qsTr("Hello World")

    Column {
        anchors.fill: parent
        spacing: 3

        Rectangle{
            id: player
            color: "#00f5f5"
            objectName: "player"
            height: parent.height / 5 * 2
            width: parent.width
            //opacity: 0.0
            //layer.enabled: false
            IcePlayer {
                id: icePlayer
            }
        }


        RowLayout {
            //anchors.fill: parent //奇怪的是会影响spacing，变得很大
            spacing: 6
            Rectangle{
                id: mytext
                color: "#aaaaaa"
                Layout.fillWidth: true
                Layout.minimumWidth: 50
                Layout.preferredWidth: 100
                Layout.maximumWidth: 100
                Layout.minimumHeight: 30
                Text{
                    color: "#f2f5f5"
                    text: "contact"
                    font.pointSize: 20
                }
            }

            LineEdit {
                id: lineEdit

                Layout.fillWidth: true
                Layout.minimumWidth: 100
                Layout.preferredWidth: 300
                Layout.maximumWidth: 300
                Layout.minimumHeight: 30
                input.font.pointSize: 20

                input.color: "blue"

                Component.onCompleted: {
                    console.log("xxxxxxxxxxxxxxLineEdit", width, parent.width, mytext.width)
                }
            }

        }


        RowLayout{
            spacing: 6

            Button {
                id: call
                text: "call"

                Layout.fillWidth: true
                Layout.preferredWidth: window.width / 2 - 3
                Layout.preferredHeight: 40

                onClicked: {
                    icePlayer.call(lineEdit.text)
                    console.log("call button pressed")
                }

                background: Rectangle {
                    color: call.down ? "#66ff53" : "#00ff00"
                    border.color: "gray"
                    border.width: 2
                    radius: 4
                }
            }


            Button {
                id: hangup
                text: "hangup"

                Layout.fillWidth: true
                Layout.preferredWidth: window.width / 2 - 3
                Layout.preferredHeight: 40

                onClicked: {
                    icePlayer.hangup()
                    console.log("hangup button pressed")
                }

                background: Rectangle {
                    color: hangup.down ? "#ff4411" : "#ff0000"
                    border.color: "gray"
                    border.width: 2
                    radius: 4
                }
            }
        }


        GridLayout {
            id: grid
            columns: 2
            rowSpacing: 2

            Rectangle{
                height: 32
                width: 80
                color: "#aaaaaa"
                Text{
                    color: "#f2f5f5"
                    text: "A ftime"
                    font.pointSize: 20
                }
            }

            Rectangle{
                height: 32
                width: window.width - 80 - 2
                color: "#999999"
                Text{
                    id: firstAudioTime
                    color: "#f2f5f5"
                    text: "yyyy-MM-dd hh:mm:ss.zzz"
                    font.pointSize: 20
                    Connections
                    {
                        target: icePlayer
                        onFirstAudioPktTime: function (str)
                        {
                            firstAudioTime.text=str;
                        }

                    }
                }
            }

            Rectangle{
                height: 32
                width: 80
                color: "#aaaaaa"
                Text{
                    color: "#f2f5f5"
                    text: "V ftime"
                    font.pointSize: 20
                }
            }

            Rectangle{
                height: 32
                width: window.width - 80 - 2
                color: "#999999"
                Text{
                    id: firstVideoTime
                    color: "#f2f5f5"
                    text: "yyyy-MM-dd hh:mm:ss.zzz"
                    font.pointSize: 20
                    Connections
                    {
                        target: icePlayer
                        onFirstVideoPktTime: function (str)
                        {
                            firstVideoTime.text=str;
                        }

                    }
                }
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
