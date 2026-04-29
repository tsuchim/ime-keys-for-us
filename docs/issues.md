# Pending Issues

v0.1.0 では細部を後回しにし、必要充分な動作とリリース準備を優先します。未解決または将来確認が必要な事項をここに蓄積します。

## v0.1.0 known limitations

- IMM-based IME control may not work perfectly for every IME/application combination.
- TSF support is not implemented yet.
- UIAccess requires signing and secure installation.
- CI artifacts may be unsigned.
- Store distribution is not the target for v0.1.0.
- winget submission is prepared but not automatic.
- Right Alt may behave differently on layouts that treat it as AltGr.
- AX keyboard layouts may need separate handling in a future release.
- No settings UI yet.

## Follow-up candidates

- Add TSF-based IME state control if IMM is insufficient.
- Validate UIAccess behavior with a signed binary installed under `C:\Program Files\ImeKeysForUS\`.
- Decide installer technology details after local WiX availability is confirmed.
- Add richer diagnostics only if manual testing shows a need.
- Consider live settings reload only if restart-after-edit proves insufficient.

## Verification notes

- 2026-04-29: `cmake -S . -B build -G "Visual Studio 17 2022" -A x64` succeeded locally.
- 2026-04-29: `cmake --build build --config Release` succeeded locally and produced `build\Release\ime-keys-for-us.exe`.
- Manual runtime keyboard tests are still required on real target applications and IMEs.
- 2026-04-29: Follow-up fixes moved IMM control out of the low-level hook path, made tray icon creation non-fatal, removed default MSI startup registration, and changed CI to install WiX explicitly.
- 2026-04-29: WiX v4.0.6 local installer build succeeded with `build\wix4\wix.exe build installer/wix/Product.wxs -d SourceDir=build\Release -o build\IME-Keys-for-US-0.1.0-x64-unsigned.msi`.
- 2026-04-29: Long-press fallback was removed and replaced with configurable same-key double-tap fallback.
- 2026-04-29: After double-tap changes, `cmake --build build --config Release` succeeded locally.
- 2026-04-29: After double-tap changes, WiX v4.0.6 local installer build succeeded.
