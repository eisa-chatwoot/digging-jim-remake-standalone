# Digging Jim 独立发行版工作记录

更新时间：2026-08-05  
范围：本地 macOS 独立发行版、相关运行时修复、图标、归属信息与 README 整理。

> 此文件记录本 Fork 的独立发行版实现过程，并作为项目文档随 `docs/zh-CN/` 一同提交。

## 当前结果

- 已可生成两个自包含的 macOS 应用包：`DiggingJim.app` 与 `DiggingJimBuilder.app`。
- 包内包含游戏资源、`LICENSE.md`、wxWidgets 与其余非系统动态库；最终用户不需要安装 Homebrew 或 wxWidgets。
- `DiggingJimBuilder.app` 内嵌 `DiggingJim` 测试程序，Builder 的 **Test** 功能可独立使用。
- 当前主应用已更新到 `/Applications/DiggingJim.app`。
- 版本号为 `2.0.1`；Finder 信息中的版权字段为 `Copyright (c) 2026 FlatWhite`。

## 完成的工作

### 1. macOS `.app` 独立打包

新增 macOS 专用 CMake 打包逻辑：

- 根目录 `CMakeLists.txt`
  - 设置项目版本为 `2.0.1`。
  - 仅在 `APPLE` 平台加载打包逻辑。
  - 新增 `DIGGING_JIM_CODESIGN_IDENTITY` 缓存变量，默认使用 ad-hoc 签名（`-`）。
- `cmake/MacOSBundle.cmake`
  - 为游戏和 Builder 设置标准 macOS Bundle 元数据、Bundle ID、版本号、Finder 图标与版权字段。
  - 创建 `macos_bundle` 目标。
  - 将 `assets/`、生成的 `.icns` 图标与 `LICENSE.md` 放入 `Contents/Resources/`。
- `cmake/MacOSBundleFixup.cmake`
  - 使用 CMake `fixup_bundle()` 复制 wxWidgets 和其他非系统动态库到 `Contents/Frameworks/`。
  - 检查 bundle 中不再保留 `/opt/homebrew/` 或 `/usr/local/` 的开发机库引用。
  - 对两个 App 执行签名并通过 `codesign --verify --deep --strict` 验证。
- `game/CMakeLists.txt` 与 `editor/CMakeLists.txt`
  - macOS 下生成 `.app`；Windows/Linux 保留原本将资源复制到可执行文件旁的逻辑。
  - Builder 构建后把游戏可执行文件嵌入 `DiggingJimBuilder.app/Contents/MacOS/DiggingJim`，供测试关卡使用。

独立发行构建命令：

```bash
brew install wxwidgets

cmake -S . -B build-macos \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF
cmake --build build-macos --target macos_bundle -j 4
```

输出位置：

```text
build-macos/bin/DiggingJim.app
build-macos/bin/DiggingJimBuilder.app
```

### 2. 高分辨率图标与 macOS 显示修复

新增图标源文件：

- `assets/icons/DiggingJim/macos_icon.png`
- `assets/icons/DiggingJimBuilder/macos_icon.png`

打包时会从高分辨率 PNG 生成对应 `.icns`。若高分辨率图标不存在，构建会退回原有 `large_png.png`。

同时修改了游戏和 Builder 的运行时图标处理：

- macOS 不再在运行时调用 `window.setIcon()`，避免覆盖 Bundle 的 `.icns`，使 Finder、Dock 与“显示简介”使用新的应用图标。
- Windows 仍保留运行时窗口图标和原生小图标设置。

### 3. 资源、设置和关卡文件路径重构

新增：

- `lib/include/Utils/Paths.h`
- `lib/src/Utils/Paths.cpp`

新增统一的路径解析策略：

| 项目 | macOS | Windows | Linux |
| --- | --- | --- | --- |
| 应用资源 | `.app/Contents/Resources/assets` | 可执行文件旁的 `assets/` | 可执行文件旁的 `assets/` |
| 设置与新关卡 | `~/Library/Application Support/Digging Jim/` | `%APPDATA%/Digging Jim/` | `$XDG_DATA_HOME/digging-jim/`，或 `~/.local/share/digging-jim/` |

相关修改：

- 游戏贴图、音效、音乐、着色器、主菜单 credits、字体、Builder 帮助文档、编辑器控件资源均改为通过 `Paths::assetPath()` 加载。
- 游戏设置不再写入应用资源目录，改为用户数据目录。
- 新建/保存的关卡改为写入用户数据目录的 `caves/`。
- 启动时仍会读取可执行文件旁旧布局中的 `caves/`，兼容已有开发版或旧用户关卡。
- Builder 的“测试”操作按实际可执行文件位置启动游戏，而不再依赖当前工作目录。

### 4. 退出时误报崩溃的修复

原因：开发者 HUD 的 FPS 和坐标显示各自使用了全局 `sf::Font`。退出时静态对象析构顺序导致 SFML/OpenGL 上下文提前释放，macOS 会将正常退出报告为异常崩溃。

修复：

- `HUD::Developer::FPSCounter` 与 `HUD::Developer::PositionDisplay` 各自持有 `m_font` 成员。
- 字体成员声明在 `sf::Text` 之前，确保文本先析构、字体后析构。

验证：重新打包后正常退出没有产生新的 macOS crash report。

### 5. 快捷键调整

- 作弊模式启动键由 `F11` 调整为 `F12`，避免 MacBook 上 F11 与系统功能键冲突。
- README 中的说明已同步为 F12。

### 6. 许可证、归属与 Finder 元数据

- `LICENSE.md` 已转换为 UTF-8，修复原有乱码字符。
- 许可证中补充：
  - 原始游戏（Persei Entertainment）
  - 上游重制版（Christopher Malcolm，2025）
  - 独立发行版的打包、图标与修复（FlatWhite，2026）
- 每个 `.app` 都会将 `LICENSE.md` 复制为 `Contents/Resources/LICENSE.md`。
- Finder 的简短版权字段按当前要求只显示：`Copyright (c) 2026 FlatWhite`。

### 7. README 调整

README 已改为以独立发行版为中心：

- 顶部标签显示目前提供的是 macOS standalone 发行包。
- 普通源码构建、非独立运行时依赖与其他平台的原始发行方式指向上游仓库：
  `https://github.com/chrismalcolm/digging-jim-remake`
- 构建章节只写 macOS `macos_bundle` 独立打包。
- Windows 与 Linux 独立包保留在 TODO，尚未实现。
- 说明了图标优化、资源路径修复和退出崩溃修复。

## 已完成验证

1. 多次成功执行：

   ```bash
   cmake --build build-macos --target macos_bundle -j 4
   ```

2. `fixup_bundle()` 已验证两个 bundle 中没有开发机 Homebrew 库引用。
3. 两个 bundle 均通过：

   ```bash
   codesign --verify --deep --strict <App>.app
   ```

4. 已把最新 `DiggingJim.app` 复制到 `/Applications/DiggingJim.app`，并验证其 `NSHumanReadableCopyright` 为：

   ```text
   Copyright (c) 2026 FlatWhite
   ```

5. 曾从干净目录完成普通 macOS 编译检查，游戏与 Builder 均能编译通过。
6. 已执行 `git diff --check`，当前受 Git 跟踪的改动没有空白错误。

## 当前限制与后续事项

### 已知限制

- macOS 的普通 `cmake --build <目录>` 虽可编译 `.app`，但不应作为发行方式使用；资源和依赖的完整处理由 `macos_bundle` 目标完成。README 已只记录后者。
- 默认是 ad-hoc 签名，适合本地使用。面向其他用户的公开发布仍应使用 Developer ID 签名并完成 Apple notarization。
- 当前包只针对构建机器的 CPU 架构；尚未生成 Universal Binary。

### 待办

- 实现 Windows 独立发行包。
- 实现 Linux 独立发行包。
- 在实际 Windows 与 Linux 环境或 CI 中构建、运行并验证共享的路径代码。
- 如需公开分发 macOS App：配置 Developer ID、提交公证、并为发布物制作 zip 或 dmg。

## 关键文件索引

| 目的 | 文件 |
| --- | --- |
| macOS 打包定义 | `cmake/MacOSBundle.cmake` |
| macOS 库修复、验证与签名 | `cmake/MacOSBundleFixup.cmake` |
| 跨平台资源/数据路径 | `lib/include/Utils/Paths.h`、`lib/src/Utils/Paths.cpp` |
| 游戏目标与 macOS Bundle | `game/CMakeLists.txt` |
| Builder 目标与内嵌测试程序 | `editor/CMakeLists.txt` |
| 退出崩溃修复 | `lib/include/HUD/Developer/*.h`、`lib/src/HUD/Developer/*.cpp` |
| F12 映射 | `lib/include/Input/Input.h` |
| 发行说明 | `README.md` |
| 完整许可证 | `LICENSE.md` |
