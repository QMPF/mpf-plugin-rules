import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import YourCo.Orders 1.0

Page {
    id: root
    
    title: qsTr("Orders")
    
    background: Rectangle {
        color: Theme ? Theme.backgroundColor : "#FFFFFF"
    }
    
    // Orders model
    OrderModel {
        id: orderModel
    service: OrdersService
    }
    
    header: ToolBar {
        background: Rectangle {
            color: Theme ? Theme.surfaceColor : "#F5F5F5"
        }
        
        RowLayout {
            anchors.fill: parent
            anchors.margins: Theme ? Theme.spacingSmall : 8
            spacing: Theme ? Theme.spacingSmall : 8
            
            Label {
                text: qsTr("Orders (%1)").arg(orderModel.count)
                font.pixelSize: 18
                font.bold: true
                color: Theme ? Theme.textColor : "#212121"
                Layout.fillWidth: true
            }
            
            // Status filter
            ComboBox {
                id: statusFilter
                model: ["All", "pending", "processing", "shipped", "delivered", "cancelled"]
                onCurrentTextChanged: {
                    orderModel.filterStatus = currentText === "All" ? "" : currentText
                }
            }
            
            Button {
                text: "+"
                onClicked: createDialog.open()
            }
        }
    }
    
    // Stats row
    RowLayout {
        id: statsRow
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: Theme ? Theme.spacingMedium : 16
        spacing: Theme ? Theme.spacingMedium : 16
        
        StatCard {
            label: qsTr("Total Orders")
            value: OrdersService.getOrderCount()
            Layout.fillWidth: true
        }
        
        StatCard {
            label: qsTr("Revenue")
            value: "$" + OrdersService.getTotalRevenue().toFixed(2)
            Layout.fillWidth: true
        }
    }
    
    // Orders list
    ListView {
        id: ordersList
        anchors.top: statsRow.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: Theme ? Theme.spacingMedium : 16
        anchors.topMargin: Theme ? Theme.spacingSmall : 8
        
        spacing: Theme ? Theme.spacingSmall : 8
        clip: true
        
        model: orderModel
        
        delegate: OrderCard {
            width: ordersList.width
            
            orderId: model.id
            customerName: model.customerName
            productName: model.productName
            quantity: model.quantity
            price: model.price
            total: model.total
            status: model.status
            createdAt: model.createdAt
            
            onClicked: {
                Navigation.push("orders/detail", {orderId: model.id})
            }
            
            onStatusChangeRequested: function(newStatus) {
                OrdersService.updateStatus(model.id, newStatus)
            }
            
            onDeleteRequested: {
                deleteDialog.orderId = model.id
                deleteDialog.open()
            }
        }
        
        // Empty state
        Label {
            anchors.centerIn: parent
            visible: orderModel.count === 0
            text: qsTr("No orders found")
            color: Theme ? Theme.textSecondaryColor : "#757575"
            font.pixelSize: 16
        }
    }
    
    // Create order dialog
    CreateOrderDialog {
        id: createDialog
        onAccepted: {
            OrdersService.createOrder(orderData)
        }
    }
    
    // Delete confirmation dialog
    Dialog {
        id: deleteDialog
        
        property string orderId: ""
        
        title: qsTr("Delete Order")
        modal: true
        standardButtons: Dialog.Yes | Dialog.No
        
        anchors.centerIn: parent
        
        Label {
            text: qsTr("Are you sure you want to delete this order?")
        }
        
        onAccepted: {
            OrdersService.deleteOrder(orderId)
        }
    }
    
    // Stat card component
    component StatCard: Rectangle {
        property string label: ""
        property string value: ""
        
        implicitHeight: 80
        radius: Theme ? Theme.radiusMedium : 8
        color: Theme ? Theme.surfaceColor : "#F5F5F5"
        
        ColumnLayout {
            anchors.centerIn: parent
            spacing: 4
            
            Label {
                text: parent.parent.label
                font.pixelSize: 12
                color: Theme ? Theme.textSecondaryColor : "#757575"
                Layout.alignment: Qt.AlignHCenter
            }
            
            Label {
                text: parent.parent.value
                font.pixelSize: 24
                font.bold: true
                color: Theme ? Theme.primaryColor : "#2196F3"
                Layout.alignment: Qt.AlignHCenter
            }
        }
    }
}
