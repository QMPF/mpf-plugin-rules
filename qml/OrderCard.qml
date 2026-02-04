import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    
    property string orderId: ""
    property string customerName: ""
    property string productName: ""
    property int quantity: 0
    property double price: 0
    property double total: 0
    property string status: ""
    property date createdAt
    
    signal clicked()
    signal statusChangeRequested(string newStatus)
    signal deleteRequested()
    
    implicitHeight: 120
    radius: Theme ? Theme.radiusMedium : 8
    color: Theme ? Theme.surfaceColor : "#F5F5F5"
    
    // Hover effect
    Rectangle {
        anchors.fill: parent
        radius: parent.radius
        color: Theme ? Theme.primaryColor : "#2196F3"
        opacity: mouseArea.containsMouse ? 0.05 : 0
        
        Behavior on opacity {
            NumberAnimation { duration: 150 }
        }
    }
    
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
    }
    
    RowLayout {
        anchors.fill: parent
        anchors.margins: Theme ? Theme.spacingMedium : 16
        spacing: Theme ? Theme.spacingMedium : 16
        
        // Main info
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 4
            
            RowLayout {
                spacing: Theme ? Theme.spacingSmall : 8
                
                Label {
                    text: root.productName
                    font.pixelSize: 16
                    font.bold: true
                    color: Theme ? Theme.textColor : "#212121"
                }
                
                // Status badge
                Rectangle {
                    implicitWidth: statusLabel.implicitWidth + 12
                    implicitHeight: 20
                    radius: 10
                    color: {
                        switch (root.status) {
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
                        text: root.status
                        font.pixelSize: 11
                        font.capitalization: Font.Capitalize
                        color: {
                            switch (root.status) {
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
                text: qsTr("Customer: %1").arg(root.customerName)
                font.pixelSize: 14
                color: Theme ? Theme.textSecondaryColor : "#757575"
            }
            
            Label {
                text: qsTr("Qty: %1 × $%2").arg(root.quantity).arg(root.price.toFixed(2))
                font.pixelSize: 13
                color: Theme ? Theme.textSecondaryColor : "#757575"
            }
            
            Label {
                text: Qt.formatDateTime(root.createdAt, "yyyy-MM-dd hh:mm")
                font.pixelSize: 12
                color: Theme ? Qt.alpha(Theme.textSecondaryColor, 0.7) : "#9E9E9E"
            }
        }
        
        // Total and actions
        ColumnLayout {
            spacing: Theme ? Theme.spacingSmall : 8
            Layout.alignment: Qt.AlignRight
            
            Label {
                text: "$" + root.total.toFixed(2)
                font.pixelSize: 20
                font.bold: true
                color: Theme ? Theme.primaryColor : "#2196F3"
                Layout.alignment: Qt.AlignRight
            }
            
            RowLayout {
                spacing: 4
                
                // Status change menu
                ComboBox {
                    id: statusCombo
                    model: ["pending", "processing", "shipped", "delivered", "cancelled"]
                    currentIndex: model.indexOf(root.status)
                    
                    implicitWidth: 100
                    
                    onActivated: function(index) {
                        if (model[index] !== root.status) {
                            root.statusChangeRequested(model[index])
                        }
                    }
                }
                
                Button {
                    text: "🗑️"
                    flat: true
                    implicitWidth: 36
                    implicitHeight: 36
                    onClicked: root.deleteRequested()
                }
            }
        }
    }
}
