import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MPF.Components 1.0

Page {
    id: root
    title: qsTr("Rules Demo")

    background: Rectangle {
        color: Theme ? Theme.backgroundColor : "#FFFFFF"
    }

    property bool httpLoading: false
    property int httpStatusCode: 0
    property string eventSubId: ""

    Component.onCompleted: {
        eventSubId = EventBus.subscribeSimple("demo/rules/**", "com.biiz.rules.demo.qml")
    }
    Component.onDestruction: {
        if (eventSubId) EventBus.unsubscribe(eventSubId)
    }

    ScrollView {
        anchors.fill: parent
        contentWidth: availableWidth

        ColumnLayout {
            width: parent.width
            spacing: 24

            // =====================================================================
            // Page Header
            // =====================================================================
            ColumnLayout {
                Layout.fillWidth: true
                Layout.margins: 24
                Layout.bottomMargin: 0
                spacing: 4

                Label {
                    text: qsTr("MPF Framework Demo")
                    font.pixelSize: 24
                    font.bold: true
                    color: Theme ? Theme.textColor : "#212121"
                }

                Label {
                    text: qsTr("Showcasing UI Components, HTTP Client, and EventBus — from Rules Plugin")
                    font.pixelSize: 14
                    color: Theme ? Theme.textSecondaryColor : "#757575"
                }
            }

            // =====================================================================
            // Section 1: UI Components Gallery
            // =====================================================================
            MPFCard {
                title: qsTr("UI Components Gallery")
                subtitle: "import MPF.Components 1.0"
                Layout.fillWidth: true
                Layout.margins: 24
                Layout.topMargin: 0

                ColumnLayout {
                    width: parent.width
                    spacing: 20

                    // --- Buttons ---
                    Label {
                        text: qsTr("MPFButton — Types")
                        font.pixelSize: 14
                        font.bold: true
                        color: Theme ? Theme.textColor : "#212121"
                    }

                    Flow {
                        Layout.fillWidth: true
                        spacing: 8
                        MPFButton { text: "Primary"; type: "primary" }
                        MPFButton { text: "Secondary"; type: "secondary" }
                        MPFButton { text: "Success"; type: "success" }
                        MPFButton { text: "Warning"; type: "warning" }
                        MPFButton { text: "Danger"; type: "danger" }
                        MPFButton { text: "Ghost"; type: "ghost" }
                    }

                    Label {
                        text: qsTr("MPFButton — Sizes")
                        font.pixelSize: 14
                        font.bold: true
                        color: Theme ? Theme.textColor : "#212121"
                    }

                    Row {
                        spacing: 8
                        MPFButton { text: "Small"; size: "small"; type: "primary" }
                        MPFButton { text: "Medium"; size: "medium"; type: "primary" }
                        MPFButton { text: "Large"; size: "large"; type: "primary" }
                    }

                    Label {
                        text: qsTr("MPFButton — States")
                        font.pixelSize: 14
                        font.bold: true
                        color: Theme ? Theme.textColor : "#212121"
                    }

                    Row {
                        spacing: 8
                        MPFButton { text: "Loading..."; loading: true; type: "primary" }
                        MPFButton { text: "Disabled"; enabled: false; type: "primary" }
                    }

                    // --- Separator ---
                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: Theme ? Qt.alpha(Theme.textSecondaryColor, 0.2) : "#E0E0E0"
                    }

                    // --- Text Fields ---
                    Label {
                        text: qsTr("MPFTextField")
                        font.pixelSize: 14
                        font.bold: true
                        color: Theme ? Theme.textColor : "#212121"
                    }

                    Column {
                        Layout.fillWidth: true
                        spacing: 12

                        MPFTextField { label: "Normal"; placeholder: "Type here..."; width: parent.width }
                        MPFTextField { label: "Required"; required: true; placeholder: "Required field"; width: parent.width }
                        MPFTextField { label: "With Error"; errorMessage: "This field is invalid"; text: "bad input"; width: parent.width }
                        MPFTextField { label: "Read Only"; readOnly: true; text: "Cannot edit this"; width: parent.width }

                        Row {
                            width: parent.width
                            spacing: 16
                            MPFTextField { label: "With Prefix"; prefix: "$"; placeholder: "0.00"; width: (parent.width - parent.spacing) / 2 }
                            MPFTextField { label: "With Suffix"; suffix: "kg"; placeholder: "Weight"; width: (parent.width - parent.spacing) / 2 }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: Theme ? Qt.alpha(Theme.textSecondaryColor, 0.2) : "#E0E0E0"
                    }

                    // --- Status Badges ---
                    Label {
                        text: qsTr("StatusBadge")
                        font.pixelSize: 14
                        font.bold: true
                        color: Theme ? Theme.textColor : "#212121"
                    }

                    Flow {
                        Layout.fillWidth: true
                        spacing: 8
                        StatusBadge { status: "info"; text: "Info" }
                        StatusBadge { status: "success"; text: "Success" }
                        StatusBadge { status: "warning"; text: "Warning" }
                        StatusBadge { status: "error"; text: "Error" }
                        StatusBadge { status: "pending"; text: "Pending" }
                        StatusBadge { status: "processing"; text: "Processing" }
                        StatusBadge { status: "shipped"; text: "Shipped" }
                        StatusBadge { status: "delivered"; text: "Delivered" }
                        StatusBadge { status: "cancelled"; text: "Cancelled" }
                    }

                    Flow {
                        Layout.fillWidth: true
                        spacing: 8
                        StatusBadge { status: "success"; outlined: true; text: "Outlined" }
                        StatusBadge { status: "info"; size: "small"; text: "Small" }
                        StatusBadge { status: "warning"; size: "large"; text: "Large" }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: Theme ? Qt.alpha(Theme.textSecondaryColor, 0.2) : "#E0E0E0"
                    }

                    // --- Loading Indicators ---
                    Label {
                        text: qsTr("MPFLoadingIndicator")
                        font.pixelSize: 14
                        font.bold: true
                        color: Theme ? Theme.textColor : "#212121"
                    }

                    RowLayout {
                        spacing: 48
                        MPFLoadingIndicator { size: "small"; text: "Small" }
                        MPFLoadingIndicator { size: "medium"; text: "Loading..." }
                        MPFLoadingIndicator { size: "large"; text: "Large" }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: Theme ? Qt.alpha(Theme.textSecondaryColor, 0.2) : "#E0E0E0"
                    }

                    // --- Cards ---
                    Label {
                        text: qsTr("MPFCard")
                        font.pixelSize: 14
                        font.bold: true
                        color: Theme ? Theme.textColor : "#212121"
                    }

                    RowLayout {
                        spacing: 12
                        Layout.fillWidth: true

                        MPFCard {
                            title: "Basic Card"
                            Layout.fillWidth: true
                            implicitHeight: 100

                            Label {
                                text: "Simple card with title"
                                color: Theme ? Theme.textSecondaryColor : "#757575"
                            }
                        }

                        MPFCard {
                            title: "Hoverable"
                            hoverable: true
                            Layout.fillWidth: true
                            implicitHeight: 100

                            Label {
                                text: "Hover over me"
                                color: Theme ? Theme.textSecondaryColor : "#757575"
                            }
                        }

                        MPFCard {
                            title: "Elevated"
                            elevated: true
                            Layout.fillWidth: true
                            implicitHeight: 100

                            Label {
                                text: "Card with shadow"
                                color: Theme ? Theme.textSecondaryColor : "#757575"
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: Theme ? Qt.alpha(Theme.textSecondaryColor, 0.2) : "#E0E0E0"
                    }

                    // --- Dialogs ---
                    Label {
                        text: qsTr("MPFDialog")
                        font.pixelSize: 14
                        font.bold: true
                        color: Theme ? Theme.textColor : "#212121"
                    }

                    Flow {
                        Layout.fillWidth: true
                        spacing: 8
                        MPFButton { text: "Info Dialog"; type: "primary"; size: "small"; onClicked: infoDialog.open() }
                        MPFButton { text: "Success Dialog"; type: "success"; size: "small"; onClicked: successDialog.open() }
                        MPFButton { text: "Warning Dialog"; type: "warning"; size: "small"; onClicked: warningDialog.open() }
                        MPFButton { text: "Danger Dialog"; type: "danger"; size: "small"; onClicked: dangerDialog.open() }
                    }
                }
            }

            // =====================================================================
            // Section 2: HTTP Client Demo
            // =====================================================================
            MPFCard {
                title: qsTr("HTTP Client Demo")
                subtitle: "mpf::http::HttpClient via DemoService"
                Layout.fillWidth: true
                Layout.margins: 24
                Layout.topMargin: 0

                Column {
                    width: parent.width
                    spacing: 12

                    MPFTextField {
                        id: urlField
                        label: qsTr("Request URL")
                        text: "https://httpbin.org/get"
                        width: parent.width
                    }

                    Row {
                        spacing: 8
                        width: parent.width

                        MPFButton {
                            text: qsTr("Send GET")
                            type: "primary"
                            loading: root.httpLoading
                            onClicked: {
                                root.httpLoading = true
                                root.httpStatusCode = 0
                                DemoService.testGet(urlField.text)
                            }
                        }

                        MPFButton {
                            text: qsTr("Send POST JSON")
                            type: "success"
                            loading: root.httpLoading
                            onClicked: {
                                root.httpLoading = true
                                root.httpStatusCode = 0
                                DemoService.testPost(urlField.text, postBodyField.text)
                            }
                        }

                        Item { width: 8 }

                        StatusBadge {
                            visible: root.httpStatusCode > 0
                            status: root.httpStatusCode < 400 ? "success" : "error"
                            text: "HTTP " + root.httpStatusCode
                        }
                    }

                    MPFTextField {
                        id: postBodyField
                        label: qsTr("POST Body (JSON)")
                        text: '{"demo": true, "plugin": "rules"}'
                        width: parent.width
                    }

                    // Response panel
                    Item {
                        width: parent.width
                        height: 200

                        Rectangle {
                            anchors.fill: parent
                            radius: 8
                            clip: true
                            color: Theme ? Qt.darker(Theme.surfaceColor, 1.03) : "#F5F5F5"
                            border.width: 1
                            border.color: Theme ? Qt.alpha(Theme.textSecondaryColor, 0.2) : "#E0E0E0"

                            Label {
                                id: responseLabel
                                anchors.top: parent.top
                                anchors.left: parent.left
                                anchors.margins: 12
                                text: qsTr("Response")
                                font.bold: true
                                color: Theme ? Theme.textColor : "#212121"
                            }

                            Label {
                                id: responseTimeLabel
                                anchors.top: parent.top
                                anchors.right: parent.right
                                anchors.margins: 12
                                text: ""
                                font.pixelSize: 12
                                color: Theme ? Theme.textSecondaryColor : "#757575"
                            }

                            Flickable {
                                id: responseFlickable
                                anchors.top: responseLabel.bottom
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.bottom: parent.bottom
                                anchors.margins: 12
                                anchors.topMargin: 8
                                clip: true
                                contentWidth: width
                                contentHeight: responseBodyText.implicitHeight
                                flickableDirection: Flickable.VerticalFlick
                                boundsBehavior: Flickable.StopAtBounds

                                Text {
                                    id: responseBodyText
                                    width: responseFlickable.width
                                    text: qsTr("Click a button to send a request...")
                                    font.family: "Consolas"
                                    font.pixelSize: 12
                                    wrapMode: Text.Wrap
                                    color: Theme ? Theme.textColor : "#212121"
                                }
                            }
                        }
                    }

                    Connections {
                        target: DemoService
                        function onHttpResponseReceived(success, statusCode, body, elapsedMs) {
                            root.httpLoading = false
                            root.httpStatusCode = statusCode
                            responseTimeLabel.text = elapsedMs + "ms"
                            responseBodyText.text = body
                        }
                    }
                }
            }

            // =====================================================================
            // Section 3: EventBus Cross-Plugin Communication
            // =====================================================================
            MPFCard {
                title: qsTr("EventBus Cross-Plugin Communication")
                subtitle: qsTr("Pub/Sub messaging between Rules and Orders plugins")
                Layout.fillWidth: true
                Layout.margins: 24
                Layout.topMargin: 0

                ColumnLayout {
                    width: parent.width
                    spacing: 16

                    // Subscription info
                    RowLayout {
                        spacing: 12
                        StatusBadge { status: "info"; text: "Subscribed: demo/rules/**" }
                        Label {
                            text: qsTr("Messages received: %1").arg(DemoService.receivedMessages.length)
                            font.pixelSize: 13
                            color: Theme ? Theme.textSecondaryColor : "#757575"
                        }
                    }

                    // Separator
                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: Theme ? Qt.alpha(Theme.textSecondaryColor, 0.2) : "#E0E0E0"
                    }

                    // Publish to Orders plugin
                    Label {
                        text: qsTr("Publish to Orders Plugin")
                        font.pixelSize: 14
                        font.bold: true
                        color: Theme ? Theme.textColor : "#212121"
                    }

                    RowLayout {
                        spacing: 8

                        MPFTextField {
                            id: crossPluginMessage
                            placeholder: qsTr("Message to send...")
                            text: "Hello from Rules!"
                            Layout.fillWidth: true
                        }

                        MPFButton {
                            text: qsTr("Publish")
                            type: "primary"
                            onClicked: {
                                var count = EventBus.publish(
                                    "demo/orders/message",
                                    {
                                        message: crossPluginMessage.text,
                                        timestamp: Date.now(),
                                        source: "Rules Plugin"
                                    },
                                    "com.biiz.rules.demo"
                                )
                                publishLog.insert(0, {
                                    time: new Date().toLocaleTimeString(),
                                    topic: "demo/orders/message",
                                    message: crossPluginMessage.text,
                                    subscribers: count
                                })
                            }
                        }
                    }

                    // Sent messages log
                    Label {
                        text: qsTr("Sent Messages")
                        font.pixelSize: 13
                        font.bold: true
                        color: Theme ? Theme.textColor : "#212121"
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 120
                        radius: 6
                        color: Theme ? Qt.darker(Theme.surfaceColor, 1.03) : "#F5F5F5"
                        border.width: 1
                        border.color: Theme ? Qt.alpha(Theme.textSecondaryColor, 0.2) : "#E0E0E0"

                        ListView {
                            anchors.fill: parent
                            anchors.margins: 8
                            clip: true
                            spacing: 4
                            model: ListModel { id: publishLog }

                            delegate: RowLayout {
                                width: ListView.view.width
                                spacing: 8

                                Label {
                                    text: model.time
                                    font.pixelSize: 11
                                    font.family: "Consolas"
                                    color: Theme ? Theme.textSecondaryColor : "#9E9E9E"
                                }
                                Label {
                                    text: model.topic
                                    font.pixelSize: 11
                                    font.family: "Consolas"
                                    color: Theme ? Theme.primaryColor : "#2196F3"
                                }
                                Label {
                                    text: model.message
                                    font.pixelSize: 11
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                    color: Theme ? Theme.textColor : "#212121"
                                }
                                Label {
                                    text: "(" + model.subscribers + " subs)"
                                    font.pixelSize: 11
                                    color: Theme ? Theme.textSecondaryColor : "#9E9E9E"
                                }
                            }

                            Label {
                                anchors.centerIn: parent
                                visible: publishLog.count === 0
                                text: qsTr("No messages sent yet")
                                font.pixelSize: 12
                                color: Theme ? Theme.textSecondaryColor : "#9E9E9E"
                            }
                        }
                    }

                    // Separator
                    Rectangle {
                        Layout.fillWidth: true
                        height: 1
                        color: Theme ? Qt.alpha(Theme.textSecondaryColor, 0.2) : "#E0E0E0"
                    }

                    // Received messages
                    RowLayout {
                        Label {
                            text: qsTr("Received Messages")
                            font.pixelSize: 13
                            font.bold: true
                            color: Theme ? Theme.textColor : "#212121"
                            Layout.fillWidth: true
                        }
                        MPFButton {
                            text: qsTr("Clear")
                            size: "small"
                            type: "ghost"
                            onClicked: DemoService.clearMessages()
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 150
                        radius: 6
                        color: Theme ? Qt.darker(Theme.surfaceColor, 1.03) : "#F5F5F5"
                        border.width: 1
                        border.color: Theme ? Qt.alpha(Theme.textSecondaryColor, 0.2) : "#E0E0E0"

                        ListView {
                            anchors.fill: parent
                            anchors.margins: 8
                            clip: true
                            spacing: 4
                            model: DemoService.receivedMessages

                            delegate: RowLayout {
                                width: ListView.view.width
                                spacing: 8

                                Label {
                                    text: modelData.timestamp || ""
                                    font.pixelSize: 11
                                    font.family: "Consolas"
                                    color: Theme ? Theme.textSecondaryColor : "#9E9E9E"
                                }
                                Label {
                                    text: modelData.topic || ""
                                    font.pixelSize: 11
                                    font.family: "Consolas"
                                    color: Theme ? Theme.primaryColor : "#2196F3"
                                }
                                Label {
                                    text: modelData.message || ""
                                    font.pixelSize: 11
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                    color: Theme ? Theme.textColor : "#212121"
                                }
                                Label {
                                    text: "from: " + (modelData.senderId || "")
                                    font.pixelSize: 11
                                    color: Theme ? Theme.textSecondaryColor : "#9E9E9E"
                                }
                            }

                            Label {
                                anchors.centerIn: parent
                                visible: DemoService.receivedMessages.length === 0
                                text: qsTr("No messages received yet.\nGo to Orders Demo and send a message!")
                                font.pixelSize: 12
                                color: Theme ? Theme.textSecondaryColor : "#9E9E9E"
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }
                }
            }

            // Bottom spacer
            Item { Layout.preferredHeight: 24 }
        }
    }

    // =====================================================================
    // Dialog instances
    // =====================================================================
    MPFDialog {
        id: infoDialog
        title: qsTr("Information")
        type: "info"
        content: Label {
            text: qsTr("This is an informational dialog from MPF.Components.")
            wrapMode: Text.Wrap
            color: Theme ? Theme.textColor : "#212121"
        }
    }

    MPFDialog {
        id: successDialog
        title: qsTr("Success")
        type: "success"
        content: Label {
            text: qsTr("Operation completed successfully!")
            wrapMode: Text.Wrap
            color: Theme ? Theme.textColor : "#212121"
        }
    }

    MPFDialog {
        id: warningDialog
        title: qsTr("Warning")
        type: "warning"
        content: Label {
            text: qsTr("Please review before continuing.")
            wrapMode: Text.Wrap
            color: Theme ? Theme.textColor : "#212121"
        }
    }

    MPFDialog {
        id: dangerDialog
        title: qsTr("Confirm Delete")
        type: "danger"
        content: Label {
            text: qsTr("This action cannot be undone. Are you sure?")
            wrapMode: Text.Wrap
            color: Theme ? Theme.textColor : "#212121"
        }
    }
}
