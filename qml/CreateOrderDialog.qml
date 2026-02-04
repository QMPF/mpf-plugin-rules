import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: root
    
    property var orderData: ({})
    
    title: qsTr("Create New Order")
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel
    
    anchors.centerIn: parent
    width: Math.min(400, parent.width - 40)
    
    background: Rectangle {
        color: Theme ? Theme.surfaceColor : "#F5F5F5"
        radius: Theme ? Theme.radiusMedium : 8
    }
    
    onOpened: {
        customerField.text = ""
        productField.text = ""
        quantityField.text = "1"
        priceField.text = ""
        customerField.forceActiveFocus()
    }
    
    onAccepted: {
        orderData = {
            customerName: customerField.text,
            productName: productField.text,
            quantity: parseInt(quantityField.text) || 1,
            price: parseFloat(priceField.text) || 0,
            status: "pending"
        }
    }
    
    ColumnLayout {
        anchors.fill: parent
        spacing: Theme ? Theme.spacingMedium : 16
        
        // Customer name
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 4
            
            Label {
                text: qsTr("Customer Name")
                font.pixelSize: 12
                color: Theme ? Theme.textSecondaryColor : "#757575"
            }
            
            TextField {
                id: customerField
                Layout.fillWidth: true
                placeholderText: qsTr("Enter customer name")
            }
        }
        
        // Product name
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 4
            
            Label {
                text: qsTr("Product Name")
                font.pixelSize: 12
                color: Theme ? Theme.textSecondaryColor : "#757575"
            }
            
            TextField {
                id: productField
                Layout.fillWidth: true
                placeholderText: qsTr("Enter product name")
            }
        }
        
        // Quantity and Price row
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme ? Theme.spacingMedium : 16
            
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4
                
                Label {
                    text: qsTr("Quantity")
                    font.pixelSize: 12
                    color: Theme ? Theme.textSecondaryColor : "#757575"
                }
                
                TextField {
                    id: quantityField
                    Layout.fillWidth: true
                    placeholderText: "1"
                    text: "1"
                    validator: IntValidator { bottom: 1 }
                    inputMethodHints: Qt.ImhDigitsOnly
                }
            }
            
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4
                
                Label {
                    text: qsTr("Unit Price ($)")
                    font.pixelSize: 12
                    color: Theme ? Theme.textSecondaryColor : "#757575"
                }
                
                TextField {
                    id: priceField
                    Layout.fillWidth: true
                    placeholderText: "0.00"
                    validator: DoubleValidator { bottom: 0; decimals: 2 }
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                }
            }
        }
        
        // Total preview
        Rectangle {
            Layout.fillWidth: true
            implicitHeight: 48
            radius: Theme ? Theme.radiusSmall : 4
            color: Theme ? Qt.alpha(Theme.primaryColor, 0.1) : "#1A2196F3"
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: Theme ? Theme.spacingSmall : 8
                
                Label {
                    text: qsTr("Total:")
                    font.pixelSize: 14
                    color: Theme ? Theme.textSecondaryColor : "#757575"
                }
                
                Item { Layout.fillWidth: true }
                
                Label {
                    text: {
                        var qty = parseInt(quantityField.text) || 0
                        var price = parseFloat(priceField.text) || 0
                        return "$" + (qty * price).toFixed(2)
                    }
                    font.pixelSize: 18
                    font.bold: true
                    color: Theme ? Theme.primaryColor : "#2196F3"
                }
            }
        }
    }
}
