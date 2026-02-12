# MPF Rules Plugin

Qt Modular Plugin Framework - Rules Plugin (Example)

## 概述

另一个示例业务插件，展示多插件共存场景。

## 依赖

- Qt 6.8+ (Core, Gui, Qml, Quick)
- mpf-sdk

## 构建

```bash
export QT_DIR=/path/to/qt6
export MPF_SDK=/path/to/mpf-sdk

cmake -B build -G Ninja \
    -DCMAKE_PREFIX_PATH="$QT_DIR;$MPF_SDK" \
    -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## 许可证

MIT License
