cmake_minimum_required(VERSION 3.21)
project(rules-plugin VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Find dependencies
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

# SDK (installed via CMAKE_PREFIX_PATH)
find_package(MPF REQUIRED)

# Plugin library
add_library(rules-plugin SHARED
    src/rules_plugin.cpp
    src/orders_service.cpp
    src/order_model.cpp
    include/rules_plugin.h
    include/orders_service.h
    include/order_model.h
)

target_include_directories(rules-plugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(rules-plugin PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    MPF::foundation-sdk
)

# QML module embedded in plugin
qt_add_qml_module(rules-plugin
    URI Biiz.Rules
    VERSION 1.0
    QML_FILES
        qml/OrdersPage.qml
        qml/OrderDetailPage.qml
        qml/OrderCard.qml
        qml/CreateOrderDialog.qml
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml/Biiz/Rules
    NO_PLUGIN
    RESOURCES ui.qrc
)

# Output to plugins directory
set_target_properties(rules-plugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)

# Install
install(TARGETS rules-plugin
    LIBRARY DESTINATION plugins
    RUNTIME DESTINATION plugins
)

install(DIRECTORY ${CMAKE_BINARY_DIR}/qml/Biiz
    DESTINATION qml
)
