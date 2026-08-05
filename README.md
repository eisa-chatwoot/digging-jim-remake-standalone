<h1 align="center">⛏️ Digging Jim</h1>

<p align="center">
  <img src="./docs/images/main_menu.png" alt="Digging Jim Main Menu" width="600">
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Distribution-Self--contained%20packages-6E56CF?style=for-the-badge" alt="Self-contained distribution packages">
  <a href="https://github.com/chrismalcolm/digging-jim-remake"><img src="https://img.shields.io/badge/Based%20on-chrismalcolm%2Fdigging--jim--remake-24292F?style=for-the-badge&logo=github&logoColor=white" alt="Based on chrismalcolm/digging-jim-remake"></a>
  <img src="https://img.shields.io/badge/C%2B%2B-17-blue?style=for-the-badge&logo=cplusplus" alt="C++17">
  <img src="https://img.shields.io/badge/SFML-3.0-red?style=for-the-badge" alt="SFML 3.0">
  <img src="https://img.shields.io/badge/Standalone-Cross--platform-6E56CF?style=for-the-badge" alt="Cross-platform standalone distribution">
</p>

<p align="center">
  An unofficial, non-commercial C++ fan remake of <strong>Digging Jim</strong> — a Boulder Dash‑style game originally developed by <em>Persei Entertainment</em> in 1999.
  <br>
  This fork focuses on self-contained, independently runnable distribution packages for supported platforms.
</p>

---

## 📦 Distribution

This repository is for self-contained, independently runnable packages.

For the original non-standalone releases, platform-specific runtime dependencies, and standard source-build experience on any platform, use the upstream [**chrismalcolm/digging-jim-remake**](https://github.com/chrismalcolm/digging-jim-remake) project.

---

## ✨ Improvements in This Fork

**Features**

- Added cross-platform completed-cave progress saves; `Play` resumes at the next unfinished cave for each `.cav` file.

**Packaging**

- Added verified Windows x64 self-contained single-file EXEs with bundled MSVC/UCRT runtime files.

**Icons**

- Unified high-resolution `app_icon.png` sources for Digging Jim and Digging Jim Builder, used to produce macOS `.icns`, Windows multi-size `.ico`, and runtime PNG icons.
- Preserved the original icon assets under `assets/icons_bak/` for reference while excluding them from standalone packages.

**Fixes**

- Fixed packaged-app resource discovery so assets load reliably outside the source tree.
- Fixed shutdown resource lifetimes to prevent the packaged app from reporting a crash after a normal exit.
- Fixed a parallel-build resource-staging race between Digging Jim and Digging Jim Builder.

---

## 🚧 TODO

- [x] Create a standalone Windows x64 distribution package.
- [ ] Create a standalone Linux distribution package.

---

## 🕹️ Gameplay

<p align="center">
  <img src="./docs/images/gameplay.png" alt="Gameplay Screenshot" width="600">
</p>

You are **Jim**. Jim digs. Jim collects diamonds. Jim tries very hard not to get crushed.

Each cave presents a grid of dirt, rocks, enemies, and glittering diamonds. To escape, Jim must collect enough diamonds to meet the **diamond quota** and reach the **exit door** — all before the **cave timer** hits zero. Simple in theory; lethal in practice.

<p align="center">
  <img src="./docs/images/screenshots.png" alt="Gameplay Screenshots" width="100%">
</p>

### What stands in Jim's way

| | Entity | Description |
| :---: | :--- | :--- |
| ![Dirt](./assets/manual/dirt.gif) | **Dirt** | The most common formation. Jim can dig through it, but it stops everything else except amoeba. |
| ![Boulder](./assets/manual/rock.gif) | **Boulder** | Falls if unsupported and rolls off unstable surfaces like diamonds, other boulders, and brick walls. Dangerous when falling, but useful for hitting enemies. Jim can push them sideways with a little effort. |
| ![Diamond](./assets/manual/diamond.gif) | **Diamond** | Jim's goal. Collect enough to meet the quota and unlock the exit. Behaves much like a boulder — watch your head. |
| ![Fragile Diamond](./assets/manual/fragdiam.gif) | **Fragile Diamond** | As valuable as a normal diamond, but shatters if it falls or is hit by a falling object. |
| ![Ore](./assets/manual/nut.gif) | **Granite Ore** | Falls like a boulder. Hit it with a boulder and a diamond will appear! |
| ![Wall](./assets/manual/wall.GIF) | **Wall** | Normal brick wall. Can be destroyed by explosions. Boulders roll off it. |
| ![Solid Wall](./assets/manual/solid.GIF) | **Solid Wall** | Reinforced wall. Cannot be destroyed in any way. |
| ![Magic Wall](./assets/manual/magicwal.gif) | **Magic Wall** | Inactive until struck by a boulder or diamond. Once active it converts boulders to diamonds and vice versa, for a cave-specific duration. After it expires it simply dissolves whatever passes through. |
| ![Expanding Wall](./assets/manual/expand.gif) | **Expanding Wall** | Comes in horizontal and vertical variants. Expands along its axis into free space — sometimes used as a trap, so watch out. |
| ![Plasma](./assets/manual/plasma.gif) | **Plasma** | Expands randomly in all directions through free space at a cave-specific speed. Often quite fast — be careful when releasing it or you may get trapped. |
| ![Amoeba](./assets/manual/amoeba.gif) | **Amoeba** | Spreads through dirt and free space at a cave-specific speed. Kills all creatures except Jim. Turns to boulders when it reaches its size limit — but if Jim manages to fully enclose it so it can no longer grow, it transforms into diamonds instead. |
| ![Protozo](./assets/manual/baddie.gif) | **Protozo** | Common cave critter. Moves through free space, turning left whenever possible. Deadly on contact with Jim. When hit by a falling object it explodes, which is useful for clearing brick walls and obstacles. |
| ![Cave Gull](./assets/manual/cavegull.gif) | **Cave Gull** | Moves through free space, turning right whenever possible. Hit one with a falling object or let amoeba reach it — its explosion yields 9 diamonds. Still as deadly as a Protozo if it reaches Jim. |
| ![Eater](./assets/manual/eater.gif) | **Eater** | A two-headed menace. Turns right through free space and will gorge itself on diamonds — get rid of them before they eat your quota. |
| ![Aggressor](./assets/manual/aggres.gif) | **Aggressor** | Rare but terrifying. Uses acute senses to actively hunt Jim. Not very clever though — complex obstacles can throw them off. |
| ![Cilia](./assets/manual/yam.gif) | **Cilia** | Moves in straight lines and picks a random direction when it hits an obstacle. |
| ![TNT](./assets/manual/tnt.gif) | **TNT Box** | Left by long-forgotten miners. Can be pushed like a boulder. When the Detonator is activated, every TNT box in the cave explodes — make sure they're in the right place first. |
| ![Detonator](./assets/manual/detonate.gif) | **Detonator** | Touch it to trigger every TNT box in the cave simultaneously. |
| ![Bomb](./assets/manual/bomb.gif) | **Drop Bomb** | Extremely sensitive. Goes off if any object hits it, or if it falls and lands on something. |
| ![Tubes](./assets/manual/tubes.GIF) | **Tubes** | Bi-directional tubes let Jim pass from either side; one-way tubes only allow entry from one end. Only Jim can move through tubes. |
| ![Start Door](./assets/manual/entrance.GIF) | **Start Door** | Jim enters the cave through here at the beginning of each level. |
| ![Exit Door](./assets/manual/exit.GIF) | **Exit Door** | Locked until Jim collects enough diamonds. Reach it to advance to the next cave — don't let it get caught in an explosion. |

### Controls

| Keyboard | Controller / Joystick | Action | Description |
| :--- | :--- | :--- | :--- |
| `Arrow Keys` / `W` `A` `S` `D` | `Analog Stick` | Move / Navigate | Move Jim. Hold against a boulder for 1 second to push it. Also navigates the Main Menu. |
| `Enter` | `A` / `Cross` / `Button 0` | Select / Confirm | Confirm menu selections. Restart cave after death. |
| `Space` | `X` / `Square` / `Button 2` | Collect Mode | Dig or collect in an adjacent tile without stepping into it. Great for grabbing diamonds safely. |
| `Tab` | `B` / `Circle` / `Button 1` | Self-Destruct | Instantly restart the cave when Jim is hopelessly trapped. |
| `Esc` | `Back` / `Select` / `Button 6` | Quit | Return to the Main Menu. |
| `P` | `Start` | Pause / Resume | Pause the cave. |

---

## ⚡ Features

- **Faithful recreation** of all 100 original Persei Entertainment caves
- **Cross-platform source** — Windows, Linux (x64 & ARM64), and macOS
- **Standalone distribution** — self-contained macOS app bundles plus Windows x64 single-file EXE packages; Linux packaging is planned
- **Controller & joystick support** added alongside original keyboard controls
- **Cave Editor** — build your own cave files with a full GUI editor (undo/redo; cave properties; test-in-game; developer mode for extended tools)
- **Original `.cav` file format** — backwards-compatible with cave files from the original 1999 game
- **Progress saves** — completed-cave progress is saved per cave file and `Play` resumes at the next unfinished cave
- **Per-cave colour theming** — hue, saturation, and luminance controls per cave
- **Animated tiles** — amoeba, magic walls, plasma, and more all animate in-game
- **Sound effects** — original sound design recreated for every entity interaction

---

## 🕵️ Cheat Mode

The original game had a cheat mode activated with `F12`. This remake keeps `F12` as the activation key and expands the available cheats.

Press `F12` to activate, then:

| Key | Action |
| :--- | :--- |
| `F1` | Go to next cave |
| `F2` | Restart current cave (new) |
| `F3` | Go to previous cave (new) |

> Cave navigation in the Main Menu also steps by 1 (instead of 5) while cheat mode is active.

---

### Progress saves

After a cave is completed, the game saves the highest completed cave for that
`.cav` file. The next launch starts **Play** at the next unfinished cave;
`Start Cave` can still be used to choose another cave manually. Progress is
stored in `progress.txt` under the per-user data directory:

| Platform | Location |
| :--- | :--- |
| macOS | `~/Library/Application Support/Digging Jim/progress.txt` |
| Windows | `%APPDATA%\Digging Jim\progress.txt` |
| Linux | `$XDG_DATA_HOME/digging-jim/progress.txt`, or `~/.local/share/digging-jim/progress.txt` |

Only completed-cave progress is saved; quitting or closing the game during a
cave restarts that cave on the next launch. Selecting a cave with cheat mode
does not save it by itself, but completing that cave still records progress;
Builder test runs do not apply or write game progress.

---

## 🔧 Cave Editor

<p align="center">
  <img src="./docs/images/editor.png" alt="Cave Editor" width="600">
</p>

The editor lets you create and edit `.cav` files — the same format used by the original 1999 game. Features include:

- Place any cave entity using a tile panel
- **Rectangle fill** — drag to fill a region; middle-click for outline, right-click for solid fill
- Scroll and zoom the cave view
- Edit cave properties (timer, diamond quota, amoeba speed, magic wall duration, colour, and more)
- Test the cave directly in the game from within the editor
- Save, open, and manage multi-cave `.cav` files

### Developer Mode

Press `Ctrl+D` in the editor to toggle developer mode. This unlocks:

- **Cave resizing** — set cave dimensions freely, from as small as 20×13 up to 255×255 (default is 50×30)
- **Additional fill modes** — Line fill and Ellipse fill (outline and solid variants)

When testing a cave from the editor with developer mode active, the game also launches in developer mode. This enables additional F-keys on top of the standard cheat mode ones (both cheat mode and developer mode must be active):

| Key | Action |
| :--- | :--- |
| `F4` | Open Cave Properties window |
| `F6` | Move camera target left |
| `F7` | Move camera target up |
| `F8` | Move camera target down |
| `F9` | Move camera target right |
| `F10` | Reset camera target to Jim |

---

## 🏗️ Building a Standalone Distribution

This repository documents only builds that create independently runnable distribution packages. For ordinary source builds or non-standalone releases on any platform, use the [upstream project](https://github.com/chrismalcolm/digging-jim-remake).

### macOS

For macOS standalone app bundles, the build machine needs macOS with Xcode Command Line Tools, CMake 3.28+, and Homebrew wxWidgets:

```bash
brew install wxwidgets
```

Build the `macos_bundle` target to embed the game assets, wxWidgets, and all non-system dynamic-library dependencies. Recipients do **not** need Homebrew or wxWidgets installed.

```bash
cmake -S . -B build-macos \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF
cmake --build build-macos --target macos_bundle -j 4
```

The resulting apps are:

- `build-macos/bin/DiggingJim.app`
- `build-macos/bin/DiggingJimBuilder.app`

The Builder app embeds a game helper, so its **Test** action also works independently. Both bundles include `LICENSE.md` and receive an ad hoc signature by default. For external distribution, configure with a Developer ID identity and notarize the finished apps:

```bash
cmake -S . -B build-macos -DBUILD_SHARED_LIBS=OFF \
  -DCMAKE_BUILD_TYPE=Release \
  -DDIGGING_JIM_CODESIGN_IDENTITY="Developer ID Application: Your Name (TEAMID)"
cmake --build build-macos --target macos_bundle -j 4
```

### Windows (x64)

Build on Windows with Git, CMake 3.28+, and Visual Studio 2022 Build Tools. In
the Visual Studio Installer, import
[`scripts/windows/BuildTools.x64.vsconfig`](./scripts/windows/BuildTools.x64.vsconfig)
to install the minimal MSVC x64/x86 tools, ARM64 host tools, and Windows SDK
needed to cross-build x64 on an ARM Windows machine.

Run the build script from a Command Prompt:

```bat
scripts\windows\build-x64.cmd
```

The script bootstraps vcpkg in `%USERPROFILE%\vcpkg` when needed, builds the
static third-party dependencies, and creates the two public distribution artifacts:

```text
build-windows\dist\DiggingJim-2.0.1-windows-x64.exe
build-windows\dist\DiggingJimBuilder-2.0.1-windows-x64.exe
```

Each EXE embeds the complete package assembled by CMake. On first launch it
extracts a versioned cache under
`%LOCALAPPDATA%\DiggingJim\single-file\` (for example,
`C:\Users\<USERNAME>\AppData\Local\DiggingJim\single-file\`); later launches
reuse it. The game and Builder share the cache, and recipients do not need
vcpkg, wxWidgets, or a separate Microsoft Visual C++ runtime.

The Windows EXEs are not code-signed yet, so SmartScreen may show a warning on
other machines. Code signing should be added before a broad public release.

### Linux

A standalone Linux package target is planned. For an ordinary source build or
a non-standalone Linux release, use the [upstream project](https://github.com/chrismalcolm/digging-jim-remake).

---

## 🙏 Acknowledgements

This project builds on [**chrismalcolm/digging-jim-remake**](https://github.com/chrismalcolm/digging-jim-remake), the open-source recreation created by Christopher Malcolm. Thank you to **Christopher Malcolm** for making that work available as the foundation for this fork. Modifications in this repository remain subject to the same [CC BY-NC-SA 4.0](./LICENSE.md) license.

---

## 🎖️ Credits

**Original Game (1999) — Persei Entertainment**
- Programming: **Peter Prøst**
- Graphics: **Robert Kjettrup**
- Sound: **Henrik Sundberg**, **Peter Prøst**
- Cave Design: **Robert Kjettrup**, **Peter Prøst**, **Anders Hansen**

**Upstream Remake (2025)**
- Recreation: **Christopher Malcolm**

**Standalone Distribution Fork (2026)**

- Packaging, application icons, feature improvements, and bug fixes: **FlatWhite**

> This project is a non-commercial fan tribute. It is not affiliated with or endorsed by Persei Entertainment. Please support the original release where possible.
