# Implementation Log

このファイルは、v0.1.0 初期実装で実際に行った手順を時系列で記録するものです。

## 1. Requirements Were Made Durable

最初に、作業中も後からでも確認できるように要求をファイル化した。

作成したファイル:

- `docs/task.md`
- `docs/issues.md`
- `docs/original-request.md`

目的:

- 開発方針を一時的な会話だけに置かない。
- v0.1.0 の範囲を明確にする。
- 後回しにする懸案を `docs/issues.md` に蓄積する。

## 2. Repository State Was Checked

作業ディレクトリは次の場所だった。

```text
C:\Users\tsuchim\Github\ime-keys-for-us
```

初回確認時点では Git repository ではなかったため、実装後に `devel` ブランチで初期化した。

## 3. Native Win32 Project Was Created

CMake project と Win32 source layout を作成した。

主なファイル:

- `CMakeLists.txt`
- `src/main.cpp`
- `src/app.h`
- `src/app.cpp`
- `src/keyboard_hook.h`
- `src/keyboard_hook.cpp`
- `src/ime_controller.h`
- `src/ime_controller.cpp`
- `src/tray_icon.h`
- `src/tray_icon.cpp`
- `src/single_instance.h`
- `src/single_instance.cpp`
- `src/synthetic_input_guard.h`
- `src/synthetic_input_guard.cpp`
- `src/app.manifest`
- `src/app-uiaccess.manifest`

## 4. Application Shell Was Implemented

`App` class が以下を担当する構成にした。

- hidden message-only window の作成。
- standard Win32 message loop。
- tray icon の登録。
- Exit menu command の処理。
- long press 判定用 timer。
- keyboard hook の install/uninstall。

## 5. Keyboard Hook State Machine Was Implemented

`KeyboardHook` class に `WH_KEYBOARD_LL` hook と Alt gesture state machine を実装した。

実装した状態:

- `Idle`
- `Undecided`
- `NormalShortcut`
- `StandaloneFallback`
- `CrossFallback`

実装した動作:

- Left Alt down / Right Alt down は最初に suppress する。
- Alt tap のまま key up した場合、Left Alt は IME OFF、Right Alt は IME ON。
- Alt を押したまま non-Alt key が押された場合、suppressed Alt down を `SendInput` で replay し、通常 shortcut として通す。
- long press threshold に到達した場合、standalone Alt down/up を `SendInput` で送る。
- opposite Alt が押された場合、second Alt に対応する standalone Alt を送る。
- synthetic input は `dwExtraInfo` marker で識別し、hook recursion を防ぐ。

定数:

```cpp
constexpr DWORD ALT_LONG_PRESS_MS = 300;
constexpr DWORD ALT_TAP_MAX_MS = 250;
```

`ALT_TAP_MAX_MS` は文書化と将来拡張用で、v0.1.0 では double-tap を実装していない。

## 6. IME Control Was Isolated

IME ON/OFF は `ImeController` に閉じ込めた。

実装:

1. `GetForegroundWindow`
2. `ImmGetDefaultIMEWnd`
3. `SendMessageW(..., WM_IME_CONTROL, IMC_SETOPENSTATUS, open ? 1 : 0)`

`IMC_SETOPENSTATUS` が SDK header で解決されない環境があったため、未定義時だけ `0x0006` として補う fallback を追加した。

## 7. Tray Icon and Single-instance Guard Were Added

Tray:

- `Shell_NotifyIconW` で tray icon を登録。
- right click / context menu で Exit menu を表示。
- Exit で `PostQuitMessage`。

Single instance:

- named mutex `Local\ImeKeysForUS.SingleInstance` を使用。
- 既存 instance がある場合、新規 process は終了する。

## 8. Documentation Was Added

作成した主な文書:

- `README.md`
- `LICENSE`
- `CHANGELOG.md`
- `AGENTS.md`
- `docs/design.md`
- `docs/keyboard-state-machine.md`
- `docs/ime-control.md`
- `docs/signing-and-uiaccess.md`
- `docs/release.md`
- `docs/winget.md`
- `docs/manual-test-checklist.md`
- `docs/pr-body.md`

## 9. Installer and Packaging Preparation Was Added

Installer:

- `installer/README.md`
- `installer/wix/Product.wxs`

Packaging:

- `packaging/README.md`
- `packaging/winget/README.md`
- `packaging/winget/tsuchim.ImeKeysForUS/README.md`
- `packaging/winget/tsuchim.ImeKeysForUS/0.1.0-template/`

winget manifest template is intentionally incomplete until GitHub Release URL and SHA256 are known.

## 10. GitHub Actions Were Added

Added workflows:

- `.github/workflows/pr-build.yml`
- `.github/workflows/release.yml`

PR build:

- runs on `windows-latest`
- configures CMake
- builds Release x64
- uploads unsigned executable artifact
- tries installer build only if WiX is available

Release build:

- runs on tag push `v*`
- builds Release x64
- stages unsigned zip
- tries installer build only if WiX is available
- uploads artifacts to GitHub Release

## 11. Local Build Was Verified

Verified commands:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Result:

```text
build\Release\ime-keys-for-us.exe
```

Manual runtime keyboard tests are still required.

## 12. Git Repository Was Initialized

The repository was initialized on branch:

```text
devel
```

Commits created:

```text
6049f67 Add project documentation and release plan
b5d40f1 Implement native Win32 IME key utility
89cd194 Add CI installer and winget packaging templates
```

## 13. GitHub Repository and PR Were Created

Created public GitHub repository:

```text
https://github.com/tsuchim/ime-keys-for-us
```

Pushed branches:

- `main`
- `devel`

Set default branch:

- `main`

Opened PR:

```text
https://github.com/tsuchim/ime-keys-for-us/pull/1
```

PR title:

```text
Build initial IME Keys for US v0.1.0
```

The PR is open and was not merged.

## 14. Remaining Work

Remaining work is tracked in:

- `docs/issues.md`
- `docs/manual-test-checklist.md`

Important remaining items:

- Manual keyboard behavior tests across target apps.
- Manual tests across Microsoft Japanese IME, Google Japanese Input if installed, and ATOK if installed.
- UIAccess signed build test under `C:\Program Files\ImeKeysForUS\`.
- WiX installer build verification in an environment with WiX installed.
- Release artifact signing and SHA256 generation before final release publication.
- winget manifest completion after GitHub Release URL and SHA256 are known.

