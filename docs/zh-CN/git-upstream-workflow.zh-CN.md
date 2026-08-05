# Git 上游维护流程

本仓库是
[`chrismalcolm/digging-jim-remake`](https://github.com/chrismalcolm/digging-jim-remake)
的独立发行版 Fork。

- `origin`：自己的仓库 `FlatWhite233/digging-jim-remake-standalone`
- `upstream`：原始重制版仓库 `chrismalcolm/digging-jim-remake`

保留 `upstream` remote 不会自动修改代码；它只是让你能够在需要时查看、审查并合并上游更新。

## 首次设置 remote

在 GitHub 创建自己的空仓库后，只需要执行一次：

```bash
git remote rename origin upstream
git remote add origin git@github.com:FlatWhite233/digging-jim-remake-standalone.git
git remote -v
```

如果此电脑尚未配置 GitHub SSH，可将上面的 `git@github.com:...` 地址替换为 HTTPS：

```bash
git remote add origin https://github.com/FlatWhite233/digging-jim-remake-standalone.git
```

正常应看到：

```text
origin    git@github.com:FlatWhite233/digging-jim-remake-standalone.git
upstream  https://github.com/chrismalcolm/digging-jim-remake.git
```

确认当前改动后，首次提交并推送：

```bash
git status
git add -A
git commit -m "Add standalone macOS distribution"
git push -u origin main
```

如果新 GitHub 仓库创建时勾选了 README、License 或 `.gitignore`，远端 `main` 已经有一个初始提交。不要用 force push 覆盖它；先拉取并合并，再推送：

```bash
git fetch origin
git merge origin/main --allow-unrelated-histories
git push -u origin main
```

## 查看上游是否有更新

以下命令只获取信息，不会修改当前分支：

```bash
git fetch upstream --prune
git log --oneline main..upstream/main
git diff --stat main...upstream/main
```

如果日志为空，说明上游没有尚未合并的新提交。

## 推荐的上游同步方式

不要直接在 `main` 合并上游。先创建专用分支，再通过 GitHub Pull Request 审查和合并：

```bash
git switch main
git pull --ff-only origin main
git fetch upstream --prune
git switch -c sync/upstream-YYYY-MM-DD
git merge upstream/main
```

将 `YYYY-MM-DD` 换成实际日期，例如 `sync/upstream-2026-08-05`。

接着解决冲突、重新构建并执行受影响平台的独立发行验证。macOS 独立包验证命令是：

```bash
cmake -S . -B build-macos \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF
cmake --build build-macos --target macos_bundle -j 4
codesign --verify --deep --strict build-macos/bin/DiggingJim.app
codesign --verify --deep --strict build-macos/bin/DiggingJimBuilder.app
```

在 Windows 上，使用以下命令构建并验证 x64 独立单文件 EXE 发行物（构建过程会
生成供启动器嵌入的内部 ZIP 载荷）：

```bat
scripts\windows\build-x64.cmd
```

完成后推送同步分支，并在 GitHub 向自己的 `main` 创建 Pull Request：

```bash
git status
git add <已解决的文件>
git commit
git push -u origin sync/upstream-YYYY-MM-DD
```

Pull Request 合并后，更新本地 `main`：

```bash
git switch main
git pull --ff-only origin main
```

## 冲突处理

合并过程中先查看冲突：

```bash
git status
git diff --name-only --diff-filter=U
```

编辑每个冲突文件，保留需要的上游改动以及独立发行版所需的改动，然后完成合并：

```bash
git add <已解决的文件>
git commit
```

如果决定放弃这次合并并回到合并前状态：

```bash
git merge --abort
```

## 实用原则

- 不要对 `main` 使用 force push。
- 解决冲突时，保留 `cmake/`、`assets/icons/` 与资源路径/打包相关改动；它们实现了本 Fork 的独立发行能力。
- 执行 `git add -A` 前先查看 `git status`。
- 不提交构建产物；`.gitignore` 已忽略 `build-macos/` 与 `build-windows/`。`docs/` 下的项目文档会随仓库提交。
- 上游更新涉及 CMake、运行时路径、资源或许可证时，务必通过 Pull Request 审查后再合并。
