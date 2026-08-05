# Upstream Workflow

This repository is an independent standalone-distribution fork of
[`chrismalcolm/digging-jim-remake`](https://github.com/chrismalcolm/digging-jim-remake).

- `origin` is this repository: `FlatWhite233/digging-jim-remake-standalone`
- `upstream` is the original remake repository: `chrismalcolm/digging-jim-remake`

Keeping an `upstream` remote does not change the project automatically. It gives
you a safe, explicit way to review and merge upstream changes when wanted.

## Initial remote setup

Run this once after creating an empty GitHub repository for this project:

```bash
git remote rename origin upstream
git remote add origin git@github.com:FlatWhite233/digging-jim-remake-standalone.git
git remote -v
```

If GitHub SSH authentication is not configured on this machine, use the HTTPS
URL instead of the `git@github.com:...` URL above:

```bash
git remote add origin https://github.com/FlatWhite233/digging-jim-remake-standalone.git
```

Expected result:

```text
origin    git@github.com:FlatWhite233/digging-jim-remake-standalone.git
upstream  https://github.com/chrismalcolm/digging-jim-remake.git
```

Commit the local work and publish the initial branch:

```bash
git status
git add -A
git commit -m "Add standalone macOS distribution"
git push -u origin main
```

If the new GitHub repository was initialized with a README, license, or
`.gitignore`, its `main` branch is not empty. Do not force-push over it. Fetch
and merge that initial commit first, resolve any conflict, then push:

```bash
git fetch origin
git merge origin/main --allow-unrelated-histories
git push -u origin main
```

## Check for upstream changes

Fetch without modifying your branch, then inspect the difference:

```bash
git fetch upstream --prune
git log --oneline main..upstream/main
git diff --stat main...upstream/main
```

An empty log means there is nothing new upstream to merge.

## Recommended upstream sync workflow

Always merge upstream work on a dedicated branch first. This leaves `main`
stable and makes the review visible as a GitHub pull request.

```bash
git switch main
git pull --ff-only origin main
git fetch upstream --prune
git switch -c sync/upstream-YYYY-MM-DD
git merge upstream/main
```

Replace `YYYY-MM-DD` with the actual date, for example
`sync/upstream-2026-08-05`.

Then resolve any conflicts, rebuild the project, and run the relevant package
checks. On macOS, build and verify the standalone apps with:

```bash
cmake -S . -B build-macos \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF
cmake --build build-macos --target macos_bundle -j 4
codesign --verify --deep --strict build-macos/bin/DiggingJim.app
codesign --verify --deep --strict build-macos/bin/DiggingJimBuilder.app
```

On Windows, build and verify the x64 standalone single-file EXE artifacts with
(the build creates an internal ZIP payload for the launchers):

```bat
scripts\windows\build-x64.cmd
```

Publish the sync branch and open a pull request into this repository's `main`:

```bash
git status
git add <resolved-files>
git commit
git push -u origin sync/upstream-YYYY-MM-DD
```

After reviewing and merging the pull request, update the local main branch:

```bash
git switch main
git pull --ff-only origin main
```

## Conflict handling

During an upstream merge, inspect the affected files:

```bash
git status
git diff --name-only --diff-filter=U
```

Edit each conflicted file, keep both the upstream change and any required
standalone-packaging change, then finish the merge:

```bash
git add <resolved-files>
git commit
```

To abandon the merge and return the branch to its pre-merge state:

```bash
git merge --abort
```

## Practical rules

- Do not force-push `main`.
- Keep `cmake/`, `assets/icons/`, and the path/packaging changes when resolving
  conflicts; they implement this fork's standalone distribution behavior.
- Review `git status` before using `git add -A`.
- Keep build output out of commits; `.gitignore` already covers `build-macos/`
  and `build-windows/`. Documentation under `docs/` is intentionally
  versioned.
- Use a pull request for upstream synchronization, especially when upstream
  changes touch CMake, runtime paths, assets, or licensing.
