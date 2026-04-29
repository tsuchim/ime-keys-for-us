# IME Keys for US

IME Keys for US is a small native Windows utility for Japanese users on US keyboards.

It maps left and right Alt taps to explicit IME OFF and IME ON operations after a short double-tap timeout. This avoids IME toggle behavior, which can be hard to reason about when switching between Word, browsers, VS Code, terminals, and other apps.

This project is licensed under the MIT License.

This project is a clean native implementation and does not include AutoHotkey or code copied from alt-ime-ahk.

## Default Key Behavior

| Input | Behavior |
|---|---|
| Left Alt single tap | IME OFF after double-tap timeout |
| Right Alt single tap | IME ON after double-tap timeout |
| Left Alt tap, then Left Alt down within timeout | Cancel IME OFF and enter normal Left Alt mode |
| Right Alt tap, then Right Alt down within timeout | Cancel IME ON and enter normal Right Alt mode |
| Left Alt + another key | Normal Left Alt shortcut |
| Right Alt + another key | Normal Right Alt shortcut |
| Right Alt held, then Left Alt | Standalone Left Alt |
| Left Alt held, then Right Alt | Standalone Right Alt |

Double-tap is implemented by treating the second same-key Alt press within the timeout as an explicit request for normal Alt behavior. If the second press is released immediately, it behaves like standalone Alt. If another key is pressed while holding it, it behaves like a normal Alt shortcut. This intentionally avoids guessing ambiguous intent after the second press.

## Settings

The double-tap timeout is loaded at startup from:

```text
%APPDATA%\ImeKeysForUS\settings.ini
```

Format:

```ini
[Keyboard]
DoubleTapMs=200
```

The default is `200 ms`. Valid values are clamped to `100-500 ms`. Shorter values reduce IME switching delay; longer values make double-tap easier. Restart the app after editing settings in v0.1.0.

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

## Manual Testing

See [docs/manual-test-checklist.md](docs/manual-test-checklist.md).

## License

MIT License. See [LICENSE](LICENSE).
