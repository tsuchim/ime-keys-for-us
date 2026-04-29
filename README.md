# IME Keys for US

IME Keys for US is a small native Windows utility for Japanese users on US keyboards.

It maps standalone left and right Alt taps to explicit IME OFF and IME ON operations. This avoids IME toggle behavior, which can be hard to reason about when switching between Word, browsers, VS Code, terminals, and other apps.

This project is licensed under the MIT License.

This project is a clean native implementation and does not include AutoHotkey or code copied from alt-ime-ahk.

## Default Key Behavior

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

## Installation Status

v0.1.0 is the first native implementation. Installer packaging is prepared, but unsigned CI artifacts are acceptable for this release. Locally signed artifacts can be produced on a development machine that has the expected code signing certificate.

## Build

Requirements:

- Windows 10 or 11
- Visual Studio 2022 Build Tools
- CMake 3.21 or later

Build x64 Release:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Expected executable:

```text
build\Release\ime-keys-for-us.exe
```

Depending on the generator layout, the executable may be under a configuration subdirectory below `build`.

## Local Signing

Local signing is optional. The build supports signing with `signtool` by certificate thumbprint:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DIME_KEYS_SIGN_AFTER_BUILD=ON
cmake --build build --config Release
```

Expected local certificate:

- Subject: `CN=tsuchim`
- Thumbprint: `A8B29BB0F03E4955B62E1CA798B6041F29812385`
- Store: `CurrentUser\My`
- EKU: Code Signing

Do not commit certificates, private keys, PFX files, certificate passwords, or secrets.

## UIAccess

Normal unsigned builds run at medium integrity and may not work over elevated administrator windows.

UIAccess-capable builds require:

- Authenticode signing
- A trusted signing certificate on the local machine
- Installation in a secure location such as `C:\Program Files\ImeKeysForUS\`
- A manifest with `uiAccess="true"`

Configure a UIAccess manifest build with:

```powershell
cmake -S . -B build-uiaccess -G "Visual Studio 17 2022" -A x64 -DIME_KEYS_UIACCESS=ON -DIME_KEYS_SIGN_AFTER_BUILD=ON
cmake --build build-uiaccess --config Release
```

Running the app itself as administrator is not the preferred long-term design. UIAccess is preferred when elevated-window support is needed.

## Known Limitations

- IMM-based IME control may not work perfectly for every IME/application combination.
- TSF support is not implemented yet.
- UIAccess requires signing and secure installation.
- CI artifacts may be unsigned.
- Store distribution is not the target for v0.1.0.
- winget submission is prepared but not automatic.
- Right Alt may behave differently on layouts that treat it as AltGr.
- AX keyboard layouts may need separate handling in a future release.
- No settings UI yet.
- No double-tap behavior yet.

## Manual Testing

See [docs/manual-test-checklist.md](docs/manual-test-checklist.md).

## License

MIT License. See [LICENSE](LICENSE).

