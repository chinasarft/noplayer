import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import "./LineEdit"


RowLayout {
    property alias prompt: id.text
    property alias text: content.text
    spacing: 3
    Rectangle {
        id: label
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
        id: content

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

