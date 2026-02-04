import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YourCo.Orders 1.0

Page {
    id: root
    
    property string orderId: ""
    property var orderData: ({})
    
    title: qsTr("Order Details")
    
    background: Rectangle {
        color: Theme ? Theme.backgroundColor : "#FFFFFF"
    }
    
    Component.onCompleted: {
        loadOrder()
    }
    
    function loadOrder() {
        orderData = OrdersService.getOrder(orderId)
    }
    
    Connections {
        target: OrdersService
        function onOrderUpdated(id) {
            if (id === root.orderId) {
                loadOrder()
            }
        }
    }
    
    ScrollView {
        anchors.fill: parent
        anchors.margins: Theme ? Theme.spacingMedium : 16
        
        ColumnLayout {
            width: root.width - 2 * (Theme ? Theme.spacingMedium : 16)
            spacing: Theme ? Theme.spacingMedium : 16
            
            // Header card
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: headerContent.implicitHeight + 32
                radius: Theme ? Theme.radiusMedium : 8
                color: Theme ? Theme.surfaceColor : "#F5F5F5"
                
                ColumnLayout {
                    id: headerContent
                    anchors.fill: parent
                    anchors.margins: Theme ? Theme.spacingMedium : 16
                    spacing: Theme ? Theme.spacingSmall : 8
                    
                    RowLayout {
                        Label {
                            text: qsTr("Order #%1").arg(root.orderId)
                            font.pixelSize: 20
                            font.bold: true
                            color: Theme ? Theme.textColor : "#212121"
                            Layout.fillWidth: true
                        }
                        
                        // Status badge
                        Rectangle {
                            implicitWidth: statusLabel.implicitWidth + 16
                            implicitHeight: 28
                            radius: 14
                            color: {
                                switch (orderData.status) {
                                case "pending": return "#FFF3E0"
                                case "processing": return "#E3F2FD"
                                case "shipped": return "#E8F5E9"
                                case "delivered": return "#E8F5E9"
                                case "cancelled": return "#FFEBEE"
                                default: return "#F5F5F5"
                                }
                            }
                            
                            Label {
                                id: statusLabel
                                anchors.centerIn: parent
                                text: orderData.status || ""
                                font.pixelSize: 13
                                font.capitalization: Font.Capitalize
                                font.bold: true
                                color: {
                                    switch (orderData.status) {
                                    case "pending": return "#E65100"
                                    case "processing": return "#1565C0"
                                    case "shipped": return "#2E7D32"
                                    case "delivered": return "#2E7D32"
                                    case "cancelled": return "#C62828"
                                    default: return "#757575"
                                    }
                                }
                            }
                        }
                    }
                    
                    Label {
                        text: qsTr("Created: %1").arg(Qt.formatDateTime(orderData.createdAt, "yyyy-MM-dd hh:mm:ss"))
                        font.pixelSize: 13
                        color: Theme ? Theme.textSecondaryColor : "#757575"
                    }
                }
            }
            
            // Customer info
            GroupBox {
                title: qsTr("Customer Information")
                Layout.fillWidth: true
                
                background: Rectangle {
                    color: Theme ? Theme.surfaceColor : "#F5F5F5"
                    radius: Theme ? Theme.radiusMedium : 8
                    y: parent.topPadding - parent.bottomPadding
                    height: parent.height - parent.topPadding + parent.bottomPadding
                }
                
                ColumnLayout {
                    anchors.fill: parent
                    spacing: Theme ? Theme.spacingSmall : 8
                    
                    Label {
                        text: orderData.customerName || ""
                        font.pixelSize: 16
                        font.bold: true
                        color: Theme ? Theme.textColor : "#212121"
                    }
                }
            }
            
            // Product info
            GroupBox {
                title: qsTr("Product Details")
                Layout.fillWidth: true
                
                background: Rectangle {
                    color: Theme ? Theme.surfaceColor : "#F5F5F5"
                    radius: Theme ? Theme.radiusMedium : 8
                    y: parent.topPadding - parent.bottomPadding
                    height: parent.height - parent.topPadding + parent.bottomPadding
                }
                
                GridLayout {
                    anchors.fill: parent
                    columns: 2
                    columnSpacing: Theme ? Theme.spacingMedium : 16
                    rowSpacing: Theme ? Theme.spacingSmall : 8
                    
                    Label { text: qsTr("Product:"); color: Theme ? Theme.textSecondaryColor : "#757575" }
                    Label { text: orderData.productName || ""; font.bold: true; color: Theme ? Theme.textColor : "#212121" }
                    
                    Label { text: qsTr("Quantity:"); color: Theme ? Theme.textSecondaryColor : "#757575" }
                    Label { text: String(orderData.quantity || 0); color: Theme ? Theme.textColor : "#212121" }
                    
                    Label { text: qsTr("Unit Price:"); color: Theme ? Theme.textSecondaryColor : "#757575" }
                    Label { text: "$" + (orderData.price || 0).toFixed(2); color: Theme ? Theme.textColor : "#212121" }
                    
                    Label { text: qsTr("Total:"); color: Theme ? Theme.textSecondaryColor : "#757575"; font.bold: true }
                    Label { 
                        text: "$" + (orderData.total || 0).toFixed(2)
                        font.pixelSize: 18
                        font.bold: true
                        color: Theme ? Theme.primaryColor : "#2196F3"
                    }
                }
            }
            
            // Actions
            RowLayout {
                Layout.fillWidth: true
                spacing: Theme ? Theme.spacingSmall : 8
                
                Button {
                    text: qsTr("Back")
                    onClicked: Navigation.pop()
                }
                
                Item { Layout.fillWidth: true }
                
                ComboBox {
                    id: statusCombo
                    model: ["pending", "processing", "shipped", "delivered", "cancelled"]
                    currentIndex: model.indexOf(orderData.status)
                    
                    onActivated: function(index) {
                        if (model[index] !== orderData.status) {
                            OrdersService.updateStatus(root.orderId, model[index])
                        }
                    }
                }
                
                Button {
                    text: qsTr("Delete")
                    onClicked: deleteDialog.open()
                    
                    background: Rectangle {
                        color: parent.pressed 
                            ? Qt.darker(Theme ? Theme.errorColor : "#F44336", 1.1)
                            : Theme ? Theme.errorColor : "#F44336"
                        radius: 4
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
    }
    
    // Delete confirmation
    Dialog {
        id: deleteDialog
        title: qsTr("Delete Order")
        modal: true
        standardButtons: Dialog.Yes | Dialog.No
        anchors.centerIn: parent
        
        Label {
            text: qsTr("Are you sure you want to delete this order?")
        }
        
        onAccepted: {
            OrdersService.deleteOrder(root.orderId)
            Navigation.pop()
        }
    }
}
