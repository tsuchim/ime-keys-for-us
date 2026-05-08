## Summary

Prepare IME Keys for US v0.1.8.

## Implemented behavior

- Left Alt release requests IME OFF immediately after a standalone tap.
- Right Alt release requests IME ON immediately after a standalone tap.
- The app no longer waits for the double-tap timeout before requesting IME ON/OFF.
- Same-key double-tap and Alt shortcut fallback restore the prior IME open status when it was captured successfully.
- Left Alt / Right Alt with another key remains a normal Alt shortcut.
- Rejected Outlook search experiments are recorded in docs, but `Shift+Space` and `VK_IME_ON` / `VK_IME_OFF` fallback paths are not part of the product implementation.

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

This release requests IME OFF/ON immediately after standalone Left Alt / Right Alt release, while preserving same-key double-tap and Alt shortcut fallback behavior. It also documents the rejected Outlook search-box experiments and keeps those diagnostic fallback paths out of the release build.
