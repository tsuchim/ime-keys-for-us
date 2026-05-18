## Summary

Prepare IME Keys for US v0.1.9.

## Implemented behavior

- Left Alt release still requests IME OFF immediately after a standalone tap.
- Right Alt release still requests IME ON immediately after a standalone tap.
- The app resolves the focused control with `GetGUIThreadInfo(...).hwndFocus` before using the IMM IME control path.
- The app falls back to the foreground HWND when focused HWND resolution fails.
- The unsafe `known_open` send-skip optimization is removed so explicit set requests still send `IMC_SETOPENSTATUS`.
- The tray icon context menu handles the `Exit` command synchronously and no longer opens a second menu on right-click.
- Rejected Outlook search experiments remain documented; `Shift+Space` and `VK_IME_ON` / `VK_IME_OFF` fallback paths are not part of the product implementation.

## Build

- CMake project targeting C++17.
- Visual Studio 2022 x64 Release build verified locally.
- Output: `build\Release\ime-keys-for-us.exe`.
- PR CI smoke-tests startup registration and MSI install/uninstall.

## Signing

- Optional local `signtool` signing by thumbprint is supported.
- CI artifacts may be unsigned.
- Signing material is not committed.

## Installer

- WiX v4 installer source is prepared under `installer/wix/`.
- CI installs WiX v4.0.6 and builds the MSI artifact.
- Installer target path is `C:\Program Files\ImeKeysForUS\`.
- MSI does not register startup by default; startup is controlled by the app under the current user.

## Manual test checklist

- See `docs/manual-test-checklist.md`.
- Manual keyboard and UIAccess tests are still required before publishing a final release.

## Known limitations

- IMM-based IME control may not work perfectly for every IME/application combination.
- TSF support is not implemented yet.
- UIAccess requires signing and secure installation.
- CI artifacts may be unsigned.
- Right Alt may behave differently on layouts that treat it as AltGr.
- AX keyboard layouts may need separate handling in a future release.
- No settings UI yet.

## Release notes draft

This bug-fix release restores IME ON/OFF in Notepad and normal controls by resolving the focused HWND before IMM control and by always sending the explicit standalone Alt set request. It also fixes tray menu `Exit` selection so the app closes with one click. It preserves the v0.1.7 immediate Alt-release behavior and does not claim to fix the new Outlook search box.
