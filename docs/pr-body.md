## Summary

Build initial native Win32 implementation of IME Keys for US v0.1.0.

## Implemented behavior

- Left Alt single tap sets IME OFF after the double-tap timeout.
- Right Alt single tap sets IME ON after the double-tap timeout.
- Left Alt tap, then Left Alt down within timeout, cancels IME OFF and enters normal Left Alt mode.
- Right Alt tap, then Right Alt down within timeout, cancels IME ON and enters normal Right Alt mode.
- Left Alt / Right Alt with another key is preserved as a normal Alt shortcut.
- Cross Alt fallback emits standalone Alt for the second Alt key.

## Build

- CMake project targeting C++17.
- Visual Studio 2022 x64 Release build verified locally.
- Output: `build\Release\ime-keys-for-us.exe`.

## Signing

- Optional local `signtool` signing by thumbprint is supported.
- CI artifacts may be unsigned.
- Signing material is not committed.

## Installer

- WiX v4 installer source is prepared under `installer/wix/`.
- CI installs WiX v4.0.6 and builds the MSI artifact.
- Installer target path is `C:\Program Files\ImeKeysForUS\`.

## Manual test checklist

- See `docs/manual-test-checklist.md`.
- Manual keyboard and UIAccess tests are still required before publishing a final release.

## Known limitations

- IMM-based IME control may not work perfectly for every IME/application combination.
- TSF support is not implemented yet.
- UIAccess requires signing and secure installation.
- CI artifacts may be unsigned.
- Store distribution is not the target for v0.1.0.
- winget submission is prepared but not automatic.
- Right Alt may behave differently on layouts that treat it as AltGr.
- AX keyboard layouts may need separate handling in a future release.
- No settings UI yet.

## Release notes draft

Initial native Win32 release of IME Keys for US. This release provides explicit IME OFF on Left Alt single tap and IME ON on Right Alt single tap after a configurable double-tap timeout, while preserving normal Alt shortcuts and treating the second same-key Alt press within the timeout as a request for normal Alt behavior.
