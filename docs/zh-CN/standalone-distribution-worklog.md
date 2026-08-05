# Digging Jim 独立发行版工作记录

更新时间：2026-08-05  
范围：macOS 与 Windows x64 独立发行版、相关运行时修复、图标、归属信息与 README 整理。

> 此文件记录本 Fork 的独立发行版实现过程，并作为项目文档随 `docs/zh-CN/` 一同提交。

## 当前结果

- 已可生成两个自包含的 macOS 应用包：`DiggingJim.app` 与 `DiggingJimBuilder.app`。
- 包内包含游戏资源、`LICENSE.md`、wxWidgets 与其余非系统动态库；最终用户不需要安装 Homebrew 或 wxWidgets。
- `DiggingJimBuilder.app` 内嵌 `DiggingJim` 测试程序，Builder 的 **Test** 功能可独立使用。
- 当前主应用已更新到 `/Applications/DiggingJim.app`。
- 版本号为 `2.0.1`；Finder 信息中的版权字段为 `Copyright (c) 2026 FlatWhite`。
- Windows x64 构建会先生成包含游戏、Builder、共享 `assets/`、`LICENSE.md` 以及 MSVC/UCRT 运行库的内部 ZIP 载荷；该 ZIP 只用于嵌入启动器，不作为对外发行物。
- 可生成两个单文件 Windows EXE：`DiggingJim-2.0.1-windows-x64.exe` 与 `DiggingJimBuilder-2.0.1-windows-x64.exe`。它们分别内嵌完整载荷，并在首次启动时解压到 `%LOCALAPPDATA%\DiggingJim\single-file\`（即 `C:\Users\<USERNAME>\AppData\Local\DiggingJim\single-file\`）的版本化缓存；游戏与 Builder 共用该缓存。
- 已在 Windows 11 ARM64 虚拟机上以 ARM64 主机工具链交叉构建并验证 x64 目标；从全新解压目录启动游戏和 Builder 均正常。
- 已加入跨平台关卡进度存档：每个 `.cav` 文件记录最高已通关关卡，主菜单的 **Play** 下次启动时从下一关继续。

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
  - macOS 下生成 `.app`；非 macOS 构建会将资源放在可执行文件旁。
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

### 2. Windows x64 单文件 EXE 独立打包

新增 Windows 专用 CMake 打包与构建入口：

- 根目录 `CMakeLists.txt`
  - 自动识别 `VCPKG_ROOT`；Windows 默认使用 `x64-windows-static-md` triplet。
  - 仅在 Windows 加载 `cmake/WindowsBundle.cmake`，并创建 `windows_bundle`、`windows_single_file` 与 `windows_release` 目标。
- `cmake/WindowsBundle.cmake`
  - 生成单个共享根目录，内含 `DiggingJim.exe`、`DiggingJimBuilder.exe`、`assets/` 与 `LICENSE.md`，使 Builder 的 **Test** 功能可找到同目录游戏程序。
  - 使用 `InstallRequiredSystemLibraries` 将 MSVC 和 UCRT 运行库与程序放在一起；第三方库通过静态链接进入 x64 可执行文件。
  - 排除 macOS 产生的 `._*` 与 `.DS_Store` 元数据文件，再生成供启动器嵌入的内部 ZIP 载荷；`windows_bundle` 负责准备并验证共享包。
  - `windows_single_file` 以内部 ZIP 为载荷构建两个品牌化 EXE；`windows_release` 生成最终的两个单文件 EXE。
- `cmake/windows-launcher/`
  - 以纯 WinAPI/C++17 实现两个 GUI 启动器，启动器自身使用静态 MSVC 运行库，资源区内嵌完整 ZIP 与对应应用图标。
  - 首次运行由 Windows PowerShell 的 `Expand-Archive` 解包到用户本地缓存，通过构建时写入的 SHA-256 版本标记复用缓存；两个外层 EXE 通过互斥锁共用一个缓存，避免并发启动重复解压。
- `cmake/WindowsSingleFileVerify.cmake`
  - 检查两个 EXE 都存在且至少包含完整 ZIP 大小，并确认其自身不依赖非系统 DLL。
- `cmake/WindowsBundleVerify.cmake`
  - 检查两个 EXE、资源、许可证和 MSVC/UCRT 运行库均在包内。
  - 检查解析出的依赖不会落到构建机上的第三方目录；Windows 系统 DLL 与 API-set 转发项被正确识别为系统组件。
- `scripts/windows/BuildTools.x64.vsconfig`
  - 记录最小 Visual Studio Build Tools 组件：MSVC x64/x86、ARM64 主机工具和 Windows 11 SDK。
- `scripts/windows/build-x64.cmd`
  - 自动选择 ARM64 或 x64 主机 MSVC、始终以 `-arch=x64` 为目标；首次克隆/引导 vcpkg，并构建 `windows_release`。
- `editor/CMakeLists.txt`
  - Builder 在所有平台依赖游戏目标；这既满足其 **Test** 操作，也避免 Windows 并行构建时两个目标同时复制同一份 `assets/` 造成“拒绝访问”。

Windows 构建命令：

```bat
scripts\windows\build-x64.cmd
```

输出位置：

```text
build-windows\dist\DiggingJim-2.0.1-windows-x64.exe
build-windows\dist\DiggingJimBuilder-2.0.1-windows-x64.exe
```

构建过程中还会在 `build-windows\dist\` 下生成 ZIP 载荷，供两个启动器嵌入；该文件不需要上传到发行页面。两个单文件 EXE 都各自包含完整载荷，首次运行会自动将内容放入用户本地缓存后再启动对应程序。按用途分发游戏 EXE 或 Builder EXE 即可。

### 3. 跨平台高分辨率图标与 macOS 显示修复

新增图标源文件：

- `assets/icons/DiggingJim/app_icon.png`
- `assets/icons/DiggingJimBuilder/app_icon.png`

这两份高分辨率 PNG 是跨平台图标源：macOS 打包从中生成 `.icns`；Windows 的 `.ico` 资源和运行时 PNG 也由同一图源生成。若高分辨率图标不存在，macOS 构建会退回 `large_png.png`。

改版前的游戏与 Builder 图标已保存在 `assets/icons_bak/DiggingJim/` 和
`assets/icons_bak/DiggingJimBuilder/`，用于参考或恢复；当前的应用图标资源只从
`assets/icons/` 读取。macOS 与 Windows 打包会显式排除 `icons_bak/`，因此备份不会
进入独立发行包。

同时修改了游戏和 Builder 的运行时图标处理：

- macOS 不再在运行时调用 `window.setIcon()`，避免覆盖 Bundle 的 `.icns`，使 Finder、Dock 与“显示简介”使用新的应用图标。
- macOS 重新打包时会先清理 Bundle 中已暂存的 `assets/`，因此重命名后的旧图标文件不会残留在应用包内。
- Windows 与 Linux 运行时优先从 `app_icon.png` 读取大图标，缺失时才回退到 `large_png.png`；Windows 仍保留原生小图标设置。
- Windows 的 `large.ico` 包含 16–256 px 的多尺寸图层，并将 256 px 图层置于首位，避免只读取首帧的跨平台查看器放大低分辨率图层；游戏、Builder 以及两个单文件启动器均使用它。

### 4. 资源、设置和关卡文件路径重构

新增：

- `lib/include/Utils/Paths.h`
- `lib/src/Utils/Paths.cpp`

新增统一的路径解析策略：

| 项目 | macOS | Windows | Linux |
| --- | --- | --- | --- |
| 应用资源 | `.app/Contents/Resources/assets` | 可执行文件旁的 `assets/` | 可执行文件旁的 `assets/` |
| 设置、进度与新关卡 | `~/Library/Application Support/Digging Jim/` | `%APPDATA%/Digging Jim/` | `$XDG_DATA_HOME/digging-jim/`，或 `~/.local/share/digging-jim/` |

相关修改：

- 游戏贴图、音效、音乐、着色器、主菜单 credits、字体、Builder 帮助文档、编辑器控件资源均改为通过 `Paths::assetPath()` 加载。
- 游戏设置不再写入应用资源目录，改为用户数据目录。
- 新建/保存的关卡改为写入用户数据目录的 `caves/`。
- 关卡进度写入用户数据目录的 `progress.txt`；只在成功通关时更新，不保存关卡中途的地图状态。
- 启动时仍会读取可执行文件旁旧布局中的 `caves/`，兼容已有开发版或旧用户关卡。
- Builder 的“测试”操作按实际可执行文件位置启动游戏，而不再依赖当前工作目录。

### 5. 关卡进度存档

- `lib/include/Utils/Paths.h` 与 `lib/src/Utils/Paths.cpp` 新增 `Paths::progressFile()`，沿用现有跨平台用户数据目录。
- `Game` 启动时读取 `progress.txt`；每次成功通过关卡时记录对应 `.cav` 文件的最高通关编号。
- 主菜单的 **Play** 自动从该文件的下一关开始；**Start Cave** 仍可手动调整起始关卡。
- 作弊模式只负责跳转关卡，不会因跳转本身写入存档；在作弊模式下实际通关仍会记录进度。
- Builder 测试模式不会应用或写入游戏进度，避免编辑器测试污染正式游戏存档。
- 进度文件采用简单的可读文本格式，损坏或不存在时自动从第一关开始，不会阻止游戏启动。

### 6. 退出时误报崩溃的修复

原因：开发者 HUD 的 FPS 和坐标显示各自使用了全局 `sf::Font`。退出时静态对象析构顺序导致 SFML/OpenGL 上下文提前释放，macOS 会将正常退出报告为异常崩溃。

修复：

- `HUD::Developer::FPSCounter` 与 `HUD::Developer::PositionDisplay` 各自持有 `m_font` 成员。
- 字体成员声明在 `sf::Text` 之前，确保文本先析构、字体后析构。

验证：重新打包后正常退出没有产生新的 macOS crash report。

### 7. 快捷键调整

- 作弊模式启动键由 `F11` 调整为 `F12`，避免 MacBook 上 F11 与系统功能键冲突。
- README 中的说明已同步为 F12。

### 8. 许可证、归属与 Finder 元数据

- `LICENSE.md` 已转换为 UTF-8，修复原有乱码字符。
- 许可证中补充：
  - 原始游戏（Persei Entertainment）
  - 上游重制版（Christopher Malcolm，2025）
  - 独立发行版的打包、图标与修复（FlatWhite，2026）
- 每个 `.app` 都会将 `LICENSE.md` 复制为 `Contents/Resources/LICENSE.md`。
- Finder 的简短版权字段按当前要求只显示：`Copyright (c) 2026 FlatWhite`。

### 9. README 调整

README 已改为以独立发行版为中心：

- 顶部标签显示 Cross-platform standalone 发行定位。
- 普通源码构建、非独立运行时依赖与其他平台的原始发行方式指向上游仓库：
  `https://github.com/chrismalcolm/digging-jim-remake`
- 构建章节记录 macOS `macos_bundle` 与 Windows x64 `windows_release` 独立打包。
- Windows x64 已在 TODO 中标记完成，Linux 独立包仍待实现。
- 说明了图标优化、资源路径修复、退出崩溃修复、关卡进度存档、Windows 单文件 EXE 打包和并行资源复制修复。

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
6. 已在 Windows 11 ARM64 虚拟机执行 `scripts\windows\build-x64.cmd`，成功产出两个单文件 EXE（构建使用的内部 ZIP 载荷也通过验证）。
7. 从内部 ZIP 载荷解压到干净目录后，确认 `DiggingJim.exe` 与 `DiggingJimBuilder.exe` 均为 `PE32+ x86-64` GUI 程序；游戏与 Builder 都可启动。
8. 直接启动两个单文件 EXE，均成功自动解压并显示游戏或 Builder；缓存标记已写入 `%LOCALAPPDATA%\DiggingJim\single-file\`（即 `C:\Users\<USERNAME>\AppData\Local\DiggingJim\single-file\`）。
9. 自动验证确认两个外层 EXE 均嵌入完整 ZIP 载荷，且仅依赖 Windows 系统 DLL；内部 ZIP 验证器确认资源、许可证和 app-local MSVC/UCRT 运行库都在载荷中，同时确认未包含 `._*` 或 `.DS_Store` macOS 元数据文件。
10. 已通过 macOS `macos_bundle` 与 Windows x64 `windows_release` 编译打包检查；编译器没有报告本次进度存档代码错误。
11. 已执行 `git diff --check`，当前受 Git 跟踪的改动没有空白错误。

## 当前限制与后续事项

### 已知限制

- macOS 的普通 `cmake --build <目录>` 虽可编译 `.app`，但不应作为发行方式使用；资源和依赖的完整处理由 `macos_bundle` 目标完成。README 已只记录后者。
- 默认是 ad-hoc 签名，适合本地使用。面向其他用户的公开发布仍应使用 Developer ID 签名并完成 Apple notarization。
- 当前 macOS 包只针对构建机器的 CPU 架构；尚未生成 Universal Binary。
- Windows 当前只提供 x64 发行物。它可由 Windows ARM64 主机构建，但不等同于 Windows ARM64 原生发行物。
- Windows 单文件 EXE 仍未代码签名；在其他机器上可能收到 SmartScreen 警告。两个 EXE 各自内嵌一份完整 ZIP 载荷，应按用途分别分发。

### 待办

- 实现 Linux 独立发行包。
- 在 CI 中重复构建、运行并验证 Windows 与未来 Linux 的独立发行物。
- 视发布需求补充 Windows 代码签名与 SmartScreen 信誉建设。
- 如需公开分发 macOS App：配置 Developer ID、提交公证、并为发布物制作 zip 或 dmg。

## 关键文件索引

| 目的 | 文件 |
| --- | --- |
| macOS 打包定义 | `cmake/MacOSBundle.cmake` |
| macOS 库修复、验证与签名 | `cmake/MacOSBundleFixup.cmake` |
| Windows 单文件 EXE 打包定义 | `cmake/WindowsBundle.cmake` |
| Windows 包完整性验证 | `cmake/WindowsBundleVerify.cmake` |
| Windows 单文件 EXE 源码与验证 | `cmake/windows-launcher/`、`cmake/WindowsSingleFileVerify.cmake` |
| Windows x64 构建入口与最小 Build Tools 配置 | `scripts/windows/build-x64.cmd`、`scripts/windows/BuildTools.x64.vsconfig` |
| 跨平台资源/数据路径 | `lib/include/Utils/Paths.h`、`lib/src/Utils/Paths.cpp` |
| 关卡进度存档 | `game/include/Game/Game.h`、`game/src/Game/Game.cpp` |
| 游戏目标与非 macOS 资源复制 | `game/CMakeLists.txt` |
| Builder 目标、共享资源排序与内嵌测试程序 | `editor/CMakeLists.txt` |
| 退出崩溃修复 | `lib/include/HUD/Developer/*.h`、`lib/src/HUD/Developer/*.cpp` |
| F12 映射 | `lib/include/Input/Input.h` |
| 发行说明 | `README.md` |
| 完整许可证 | `LICENSE.md` |
