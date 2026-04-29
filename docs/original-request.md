# Original Request

このファイルは、初回開発依頼の要求を後から確認できるように整理した記録です。

## Repository Identity

- GitHub owner: `tsuchim`
- Repository name: `ime-keys-for-us`
- Product name: `IME Keys for US`
- Executable name: `ime-keys-for-us.exe`
- Visual Studio solution name: `ImeKeysForUS.sln`
- C++ project name: `ImeKeysForUS`
- winget package identifier: `tsuchim.ImeKeysForUS`
- License: MIT
- Initial development branch: `devel`
- PR target branch: `main`

## Product Goal

Japanese users on US keyboards need explicit IME ON and IME OFF keys, not an IME toggle.

The product is a clean native Windows replacement for AutoHotkey-based Alt IME utilities. It maps physically thumb-accessible Alt keys on US keyboards to explicit Japanese IME state:

- Left Alt tap: IME OFF / English
- Right Alt tap: IME ON / Japanese

The app must remain narrow in scope and must not become a general hotkey manager.

## Background and Motivation

- IME toggle state is hard to reason about.
- Windows and applications do not always preserve IME state consistently across applications.
- Users switch between Word, browsers, VS Code, terminals, and other apps.
- Users should not need to remember whether the current app is in Japanese mode or English mode.
- US keyboards do not have JIS-style `無変換` and `変換` thumb keys.
- Left and right Alt keys are physically close to old Japanese thumb-key positions.

## Clean-room Requirement

This project must be a clean native implementation.

Do not copy code from:

- `alt-ime-ahk`
- `alt-ime-ahk-v2f`
- `US-AltIME.ahk`
- any other AutoHotkey IME script
- any GPL implementation

It is acceptable to implement the same user-facing idea from scratch using Windows API documentation and original implementation.

Do not translate AutoHotkey IME helper functions into C++ line-by-line.

## v0.1.0 Behavior

| Input | Behavior |
|---|---|
| Left Alt tap | IME OFF |
| Right Alt tap | IME ON |
| Left Alt + another key | Normal Left Alt shortcut |
| Right Alt + another key | Normal Right Alt shortcut |
| Left Alt long press | Standalone Left Alt |
| Right Alt long press | Standalone Right Alt |
| Right Alt held, then Left Alt | Standalone Left Alt |
| Left Alt held, then Right Alt | Standalone Right Alt |

Normal Alt shortcuts that must keep working:

- Alt+F
- Alt+Space
- Alt+Tab
- Alt+Enter
- Alt+Left
- Alt+Right
- Alt+underlined menu key

Double-tap behavior is explicitly out of scope for v0.1.0.

## Implementation Requirements

- C++17 or later.
- Win32 API.
- No AutoHotkey runtime.
- No .NET.
- No Electron.
- No WebView.
- No networking.
- No telemetry.
- No updater in v0.1.0.
- Use `WH_KEYBOARD_LL` for low-level keyboard monitoring.
- Use `SendInput` only when replaying suppressed Alt input.
- Use a standard Win32 message loop.
- Include a tray icon with an Exit menu.
- Include a single-instance guard.
- Keep the keyboard hook callback fast.

## Timing Constants

Define these constants in one obvious location:

```cpp
ALT_LONG_PRESS_MS = 300
ALT_TAP_MAX_MS = 250
```

`ALT_TAP_MAX_MS` may be used for classification and documentation, but v0.1.0 must not implement double-tap behavior.

## IME Control

Implement IME ON/OFF using the Windows IMM approach first:

1. Get the foreground window.
2. Get the default IME window with `ImmGetDefaultIMEWnd`.
3. Send `WM_IME_CONTROL` with `IMC_SETOPENSTATUS`.
4. OFF = `0`.
5. ON = `1`.

IME control must be isolated behind a small interface so TSF support can be added later.

## TSF

Do not implement TSF in v0.1.0.

Add documentation explaining that future versions may add TSF-based IME state control if IMM-based control is insufficient.

## UIAccess and Signing

For v0.1.0:

- Prepare for UIAccess.
- Do not require administrator elevation for normal runtime.
- Include an application manifest for normal mode.
- Include a build option or manifest variant for `uiAccess="true"`.

UIAccess requirements:

- Authenticode signing.
- Secure installation location such as `C:\Program Files\ImeKeysForUS\`.
- Signing certificate trusted on the local machine.
- Manifest with `uiAccess="true"`.

Use the existing local code signing certificate when building signed local artifacts:

- Subject: `CN=tsuchim`
- Thumbprint: `A8B29BB0F03E4955B62E1CA798B6041F29812385`
- Store: `CurrentUser\My`
- EKU: Code Signing

Do not commit certificates, private keys, PFX files, certificate passwords, or secrets.

## Git and Release Workflow

- Work on branch `devel`.
- PR target branch is `main`.
- Make small commits.
- Do not merge to `main`.
- Push `devel`.
- Open a PR from `devel` to `main`.
- PR title: `Build initial IME Keys for US v0.1.0`.
- Do not auto-merge.

## Distribution Preparation

Prepare:

- MIT License.
- README.
- CHANGELOG.
- CMake build.
- Installer directory and WiX-oriented installer work.
- GitHub Actions PR build workflow.
- GitHub Actions release workflow.
- winget documentation and incomplete template.
- Manual test checklist.
- Release process documentation.

winget submission is prepared but must not be submitted automatically in v0.1.0.

## Acceptance Criteria

- Repository has MIT license.
- Project builds on Windows.
- CMake or Visual Studio build works.
- Native Win32 executable exists.
- Keyboard hook implementation exists.
- IME controller implementation exists.
- Tray icon and Exit menu exist.
- Single-instance guard exists.
- Normal Alt shortcuts are preserved by design.
- Long press fallback exists.
- Cross Alt fallback exists.
- Double-tap is not implemented.
- Installer exists or installer work is clearly documented with build steps.
- GitHub Actions PR build exists.
- Release workflow exists.
- Signing is optional and documented.
- UIAccess requirements are documented.
- winget preparation docs and manifest template exist.
- Manual test checklist exists.
- PR from `devel` to `main` is opened.

