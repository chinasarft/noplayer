import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import OpenGLUnderQML 1.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import "qrc:/ui/"

Window {
    id: window
    visible: true
    width: 360
    height: 640
    color: "#000000"
    title: qsTr("ice player")

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

        ComboBox {
            id: soureType
            editable: false
            currentIndex: 0
            model: ListModel {
                id: cbItems
                ListElement { text: "tutk"; color: "Yellow" }
                ListElement { text: "file"; color: "Green" }
                ListElement { text: "h264/pcmu file"; color: "Brown" }
                ListElement { text: "h264/aac"; color: "Red" }
            }
            textRole: "text"
            width: parent.width
            onCurrentIndexChanged: {
                console.debug(cbItems.get(currentIndex).text + ", " + cbItems.get(currentIndex).color)
            }
        }

        Column {
            spacing: 3
            PickFile {
                visible: soureType.currentIndex != 0
                id: avfile1
                text: soureType.currentIndex == 1 ? "/Users/liuye/Downloads/1517444052127-1517444059607.ts" :"/Users/liuye/qbox/linking/link/libtsuploader/pcdemo/material/h265_aac_1_16000_pcmu_8000.mulaw"
                promptText: soureType.currentIndex == 1 ? "浏览":"音频文件"
            }

            PickFile {
                visible: soureType.currentIndex > 1
                text: "/Users/liuye/qbox/linking/link/libtsuploader/pcdemo/material/h265_aac_1_16000_h264.h264"
                promptText: "视频文件"
                id: avfile2
            }
        }

        RowLayout {
            visible: soureType.currentIndex == 0
            //anchors.fill: parent //奇怪的是会影响spacing，变得很大
            spacing: 6
            Rectangle{
                id: idlabel
                color: "#aaaaaa"
                Layout.fillWidth: true
                Layout.minimumWidth: 50
                Layout.preferredWidth: 100
                Layout.maximumWidth: 100
                Layout.minimumHeight: 30
                Text{
                    color: "#f2f5f5"
                    text: "id"
                    font.pointSize: 20
                }
            }

            LineEdit {
                id: myid

                Layout.fillWidth: true
                Layout.minimumWidth: 100
                Layout.preferredWidth: 300
                Layout.maximumWidth: 300
                Layout.minimumHeight: 30
                input.font.pointSize: 20

                input.color: "blue"
                text: "CVUUBN1MP9BWAN6GU1MJ"//"C3PA9N1WYDBCAMPGU1Y1" //"CVUUBN1MP9BWAN6GU1MJ"

                Component.onCompleted: {
                    console.log("xxxxxxxxxxxxxxLineEdit", width, parent.width, myid.width)
                }
            }

        }

        RowLayout {
            visible: soureType.currentIndex == 0
            //anchors.fill: parent //奇怪的是会影响spacing，变得很大
            spacing: 6
            Rectangle{
                id: pwdlabel
                color: "#aaaaaa"
                Layout.fillWidth: true
                Layout.minimumWidth: 50
                Layout.preferredWidth: 100
                Layout.maximumWidth: 100
                Layout.minimumHeight: 30
                Text{
                    color: "#f2f5f5"
                    text: "pwd"
                    font.pointSize: 20
                }
            }

            LineEdit {
                id: mypwd

                Layout.fillWidth: true
                Layout.minimumWidth: 100
                Layout.preferredWidth: 300
                Layout.maximumWidth: 300
                Layout.minimumHeight: 30
                input.font.pointSize: 20

                input.color: "blue"
                text: "8888iIJj"

                Component.onCompleted: {
                    console.log("xxxxxxxxxxxxxxLineEdit", width, parent.width, mypwd.width)
                }
            }

        }



        RowLayout{
            spacing: 6

            Button {
                id: call
                text: "play"

                Layout.fillWidth: true
                Layout.preferredWidth: window.width / 3 - 3
                Layout.preferredHeight: 40

                onClicked: {
                    if (soureType.currentIndex === 0)
                        icePlayer.play(soureType.currentIndex, myid.text, mypwd.text)
                    else {
                        icePlayer.play(soureType.currentIndex, avfile1.text, avfile2.text)
                        console.log("play button pressed", avfile1.text, avfile2.text)
                    }
                }

                background: Rectangle {
                    color: call.down ? "#66ff53" : "#00ff00"
                    border.color: "gray"
                    border.width: 2
                    radius: 4
                }
            }

            Button {
                id: audio
                text: "playaudio"

                Layout.fillWidth: true
                Layout.preferredWidth: window.width / 3 - 3
                Layout.preferredHeight: 40

                onClicked: {
                    if (soureType.currentIndex === 0)
                        icePlayer.playAudio()
                }

                background: Rectangle {
                    color: call.down ? "#66ff53" : "#00ff00"
                    border.color: "gray"
                    border.width: 2
                    radius: 4
                }
            }

            Button {
                id: stop
                text: "stop"

                Layout.fillWidth: true
                Layout.preferredWidth: window.width / 3 - 3
                Layout.preferredHeight: 40

                onClicked: {
                    firstAudioTime.text="yyyy-MM-dd hh:mm:ss.zzz"
                    firstVideoTime.text="yyyy-MM-dd hh:mm:ss.zzz"
                    icePlayer.stop()
                    console.log("stop button pressed")
                }

                background: Rectangle {
                    color: stop.down ? "#ff4411" : "#ff0000"
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
                        onGetFirstAudioPktTime:
                        {
                            firstAudioTime.text=timestr;
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
                        onGetFirstVideoPktTime:
                        {
                            firstVideoTime.text=timestr;
                        }

                    }
                }
            }

        }

        Rectangle{
            height: 30
            width: parent.width
            color: "#bbbbbb"
            Text{
                id: streamInfo
                color: "#f2f5f5"
                text: "--------------"
                font.pointSize: 15
                Connections
                {
                    target: icePlayer
                    onStreamInfoUpdate:
                    {
                        streamInfo.text=infoStr;
                    }

                }
            }
        }

    }



    onClosing: {
        console.log("closesssssssssssss")
        icePlayer.stop()
        Qt.quit()
    }

    Component.onCompleted: {
        console.log("yyyyyyyyyyyfrom qml")
    }

}
