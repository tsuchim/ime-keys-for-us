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
- keyboard timer。
- keyboard hook の install/uninstall。

## 5. Keyboard Hook State Machine Was Implemented

`KeyboardHook` class に `WH_KEYBOARD_LL` hook と Alt gesture state machine を実装した。

実装した状態:

- `Idle`
- `AltDownHeld`
- `PendingTap`
- `NormalShortcut`
- `CrossFallback`

実装した動作:

- Left Alt down / Right Alt down は最初に suppress する。
- Alt tap のまま key up した場合、pending tap として double-tap timeout を待つ。
- Alt を押したまま non-Alt key が押された場合、suppressed Alt down を `SendInput` で replay し、通常 shortcut として通す。
- double-tap timeout 以内に同じ Alt の 2 回目の down が来た場合、pending IME を cancel し、その Alt down を `SendInput` で replay して通常 Alt mode に入る。
- pending tap が timeout した場合、Left Alt は IME OFF、Right Alt は IME ON を request する。
- opposite Alt が押された場合、second Alt に対応する standalone Alt を送る。
- synthetic input は `dwExtraInfo` marker で識別し、hook recursion を防ぐ。

定数:

```cpp
constexpr DWORD DEFAULT_ALT_DOUBLE_TAP_MS = 100;
constexpr DWORD MIN_ALT_DOUBLE_TAP_MS = 100;
constexpr DWORD MAX_ALT_DOUBLE_TAP_MS = 500;
```

`DoubleTapMs` は `%APPDATA%\ImeKeysForUS\settings.ini` から起動時に読み込む。

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

## 15. Follow-up Fixes Before Merge

GHQ reviewed PR #1 and recommended not merging until several v0.1.0 readiness issues were fixed. The findings were accepted as valid.

Applied fixes:

- Moved IME control out of the low-level keyboard hook path.
- Added `src/app_messages.h` with `WM_APP_SET_IME_OFF` and `WM_APP_SET_IME_ON`.
- Changed `KeyboardHook` so Alt tap classification posts a custom message to the hidden app window.
- Changed `App::HandleMessage()` so it calls `ImeController::SetOpenStatus()` from the normal message loop.
- Made tray icon creation non-fatal.
- Changed startup order to create the hidden window, install the keyboard hook, start the keyboard timer, then add the tray icon.
- Removed unused `synthetic_alt_down_sent` state.
- Removed default HKCU Run startup registration from the per-machine MSI.
- Updated installer documentation to describe manual startup options.
- Updated GitHub Actions workflows to install WiX v4 explicitly with `dotnet tool install --global wix --version 4.0.6`.
- Updated release workflow artifact names to derive the version from the Git tag.
- Updated PR workflow MSI artifact names to derive the version from `CMakeLists.txt`.

## 16. PR Review Comments

Copilot review comments on PR #1 were handled as follows:

- Accepted: pass the actual `HWND` through `App::HandleMessage()` and use it for `DefWindowProcW`.
- Accepted: release the single-instance mutex only when the process owns it.
- Accepted: call `PostMessageW(hwnd, WM_NULL, 0, 0)` after tray context menu tracking.
- Accepted: compare tray callback `lParam` directly to `WM_CONTEXTMENU` and `WM_RBUTTONUP`.
- Accepted: use scan-code based `SendInput` consistently with `KEYEVENTF_SCANCODE`.
- Already handled: remove per-machine MSI default HKCU Run registration.
- Accepted: remove unused `IME_KEYS_VERSION` compile definition.
- Already handled and kept: release workflow artifact names derive from the Git tag.
- Accepted: PR workflow MSI artifact name derives from `CMakeLists.txt`.

Behavior after review fixes:

- Left Alt single tap = IME OFF after timeout.
- Right Alt single tap = IME ON after timeout.
- Alt + another key = normal Alt shortcut.
- Second same-key Alt press within timeout = normal Alt mode.
- Cross Alt = standalone Alt fallback.

## 17. Double-tap Fallback Replaced Long-press

GHQ rejected the long-press fallback because a user can hold Alt briefly while deciding the next shortcut key. If the app emits standalone Alt during that pause, an intended `Alt+key` shortcut can accidentally become menu activation.

Applied changes:

- Removed `ALT_LONG_PRESS_MS`.
- Removed `TickLongPress()`.
- Removed long-press state handling.
- Added `src/settings.h` and `src/settings.cpp`.
- Added `AppSettings` and startup loading from `%APPDATA%\ImeKeysForUS\settings.ini`.
- Changed `DEFAULT_ALT_DOUBLE_TAP_MS` to `100`.
- Added accepted setting clamp range `100-500 ms`.
- Changed timer handling so the app arms a timer only while a pending tap exists.
- Changed `DoubleTapMs` parsing to accept only strict numeric values; invalid values use the default.
- Changed first Alt tap to create a pending tap instead of immediately changing IME.
- Changed timeout resolution to post the existing IME ON/OFF app messages.
- Changed same-key double-tap within timeout to cancel pending IME and enter normal Alt mode without changing IME state.
- Kept cross Alt fallback.
- Documented that an opposite Alt gesture cancels an existing pending tap and is treated independently.

Behavior now:

- Left Alt single tap = IME OFF after double-tap timeout.
- Right Alt single tap = IME ON after double-tap timeout.
- Second Left Alt down within timeout = cancel pending IME OFF and enter normal Left Alt mode.
- Second Right Alt down within timeout = cancel pending IME ON and enter normal Right Alt mode.
- Alt + another key = normal Alt shortcut.
- Cross Alt = standalone Alt fallback.
- Holding Alt alone does not emit standalone Alt.
