cmake_minimum_required(VERSION 3.21)
project(rules-plugin VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# macOS RPATH settings for plugins (shared libraries)
if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_INSTALL_RPATH "@loader_path;@loader_path/../lib;@loader_path/../Frameworks")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF)
endif()

# Find dependencies
find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Network)

# SDK (installed via CMAKE_PREFIX_PATH)
find_package(MPF REQUIRED)
find_package(MPFHttpClient REQUIRED)

# Plugin library
add_library(rules-plugin SHARED
    src/rules_plugin.cpp
    src/rules_service.cpp
    src/rule_model.cpp
    src/demo_service.cpp
    include/rules_plugin.h
    include/rules_service.h
    include/rule_model.h
    include/demo_service.h
)

target_include_directories(rules-plugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(rules-plugin PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::Network
    MPF::foundation-sdk
    MPF::mpf-http-client
)

# Static link CRT on MinGW to avoid cross-DLL heap issues
if(MINGW)
    target_link_options(rules-plugin PRIVATE -static-libgcc -static-libstdc++)
endif()

# QML 文件
set(PLUGIN_QML_FILES qml/RulesPage.qml qml/RuleCard.qml qml/CreateRuleDialog.qml qml/DemoPage.qml)

# Auto-flatten paths: qml/Foo.qml -> Foo.qml
foreach(file ${PLUGIN_QML_FILES})
    string(REGEX REPLACE "^qml/" "" alias "${file}")
    set_source_files_properties(${file} PROPERTIES QT_RESOURCE_ALIAS ${alias})
endforeach()

# QML module embedded in plugin
qt_add_qml_module(rules-plugin
    URI Biiz.Rules
    VERSION 1.0
    RESOURCE_PREFIX /
    QML_FILES ${PLUGIN_QML_FILES}
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
