import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: root
    
    title: qsTr("Rules")
    
    background: Rectangle {
        color: Theme ? Theme.backgroundColor : "#FFFFFF"
    }
    
    // Rules model
    RuleModel {
        id: rulesModel
        service: RulesService
    }
    
    header: ToolBar {
        background: Rectangle {
            color: "transparent"
        }

        RowLayout {
            anchors.fill: parent
            anchors.margins: Theme ? Theme.spacingSmall : 8
            spacing: Theme ? Theme.spacingSmall : 8

            // 计数标签（标题已在宿主 Header 显示，不再重复）
            Label {
                text: qsTr("%1 rules").arg(rulesModel.count)
                font.pixelSize: 13
                color: Theme ? Theme.textSecondaryColor : "#757575"
            }

            Item { Layout.fillWidth: true }

            // Status filter
            ComboBox {
                id: statusFilter
                model: ["All", "pending", "processing", "shipped", "delivered", "cancelled"]
                onCurrentTextChanged: {
                    rulesModel.filterStatus = currentText === "All" ? "" : currentText
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
            label: qsTr("Total Rules")
            value: RulesService.getRuleCount()
            Layout.fillWidth: true
        }
        
        StatCard {
            label: qsTr("Active")
            value: "$" + RulesService.getTotalRevenue().toFixed(2)
            Layout.fillWidth: true
        }
    }
    
    // Rules list
    ListView {
        id: rulesList
        anchors.top: statsRow.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: Theme ? Theme.spacingMedium : 16
        anchors.topMargin: Theme ? Theme.spacingSmall : 8
        
        spacing: Theme ? Theme.spacingSmall : 8
        clip: true
        
        model: rulesModel
        
        delegate: RuleCard {
            width: rulesList.width
            
            ruleId: model.id
            customerName: model.customerName
            productName: model.productName
            quantity: model.quantity
            price: model.price
            total: model.total
            status: model.status
            createdAt: model.createdAt
            
            onClicked: {
                detailPopup.ruleId = model.id
                detailPopup.open()
            }
            
            onStatusChangeRequested: function(newStatus) {
                RulesService.updateStatus(model.id, newStatus)
            }
            
            onDeleteRequested: {
                deleteDialog.ruleId = model.id
                deleteDialog.open()
            }
        }
        
        // Empty state
        Label {
            anchors.centerIn: parent
            visible: rulesModel.count === 0
            text: qsTr("No rules found")
            color: Theme ? Theme.textSecondaryColor : "#757575"
            font.pixelSize: 16
        }
    }
    
    // Detail popup
    Popup {
        id: detailPopup
        
        property string ruleId: ""
        property var ruleData: ({})
        
        modal: true
        anchors.centerIn: parent
        width: Math.min(500, root.width - 48)
        height: Math.min(500, root.height - 48)
        padding: 24
        
        background: Rectangle {
            color: Theme ? Theme.surfaceColor : "#FFFFFF"
            radius: Theme ? Theme.radiusMedium : 12
        }
        
        onOpened: {
            ruleData = RulesService.getRule(ruleId)
        }
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 16
            
            RowLayout {
                Layout.fillWidth: true
                
                Label {
                    text: qsTr("Rule #%1").arg(detailPopup.ruleId)
                    font.pixelSize: 20
                    font.bold: true
                    color: Theme ? Theme.textColor : "#212121"
                    Layout.fillWidth: true
                }
                
                Button {
                    text: "✕"
                    flat: true
                    onClicked: detailPopup.close()
                }
            }
            
            RowLayout {
                Label { text: qsTr("Status:"); color: Theme ? Theme.textSecondaryColor : "#757575" }
                Label { 
                    text: detailPopup.ruleData.status || ""
                    font.bold: true
                    color: Theme ? Theme.primaryColor : "#2196F3"
                }
            }
            
            RowLayout {
                Label { text: qsTr("Name:"); color: Theme ? Theme.textSecondaryColor : "#757575" }
                Label { text: detailPopup.ruleData.customerName || ""; color: Theme ? Theme.textColor : "#212121" }
            }
            
            RowLayout {
                Label { text: qsTr("Description:"); color: Theme ? Theme.textSecondaryColor : "#757575" }
                Label { text: detailPopup.ruleData.productName || ""; color: Theme ? Theme.textColor : "#212121" }
            }
            
            Item { Layout.fillHeight: true }
            
            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                
                ComboBox {
                    model: ["pending", "processing", "shipped", "delivered", "cancelled"]
                    currentIndex: model.indexOf(detailPopup.ruleData.status)
                    
                    onActivated: function(index) {
                        if (model[index] !== detailPopup.ruleData.status) {
                            RulesService.updateStatus(detailPopup.ruleId, model[index])
                            detailPopup.ruleData = RulesService.getRule(detailPopup.ruleId)
                        }
                    }
                }
                
                Item { Layout.fillWidth: true }
                
                Button {
                    text: qsTr("Delete")
                    onClicked: {
                        deleteDialog.ruleId = detailPopup.ruleId
                        deleteDialog.open()
                        detailPopup.close()
                    }
                }
                
                Button {
                    text: qsTr("Close")
                    onClicked: detailPopup.close()
                }
            }
        }
    }
    
    // Create dialog
    CreateRuleDialog {
        id: createDialog
        onAccepted: {
            RulesService.createRule(ruleData)
        }
    }
    
    // Delete confirmation dialog
    Dialog {
        id: deleteDialog
        
        property string ruleId: ""
        
        title: qsTr("Delete Rule")
        modal: true
        standardButtons: Dialog.Yes | Dialog.No
        
        anchors.centerIn: parent
        
        Label {
            text: qsTr("Are you sure you want to delete this rule?")
        }
        
        onAccepted: {
            RulesService.deleteRule(ruleId)
        }
    }
    
    // Stat card component
    component StatCard: Rectangle {
        property string label: ""
        property string value: ""

        implicitHeight: 80
        radius: Theme ? Theme.radiusMedium : 8
        color: Theme ? Theme.surfaceColor : "#F5F5F5"
        border.width: 1
        border.color: Theme ? Qt.alpha(Theme.textSecondaryColor, 0.3) : "#E0E0E0"
        
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
