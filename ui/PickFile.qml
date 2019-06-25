import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3


RowLayout {
    property alias text: labels.text
    property alias promptText: openBtn.text
    Button {
        id:openBtn
        height: 25
        text:qsTr("浏览...")
        onClicked: {
            fds.open();
        }
    }
    Label {
        id: labels
        text: qsTr("")
        height: 25
        color: "white"
    }

    FileDialog {
        id:fds
        title: "选择文件"
        folder: shortcuts.desktop
        selectExisting: true
        selectFolder: false
        selectMultiple: false
        //nameFilters: ["json文件 (*.json)"]
        onAccepted: {
            labels.text = fds.fileUrl;
            console.log("You chose: " + fds.fileUrl);
        }

        onRejected: {
            labels.text = "";
            console.log("Canceled");
        }

    }
}

