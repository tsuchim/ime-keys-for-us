# IME Keys for US

IME Keys for US is a small native Windows utility for Japanese users on US keyboards.

It maps Left Alt and Right Alt to explicit IME OFF and IME ON behavior. It avoids IME toggle semantics so users do not need to remember the current IME state while moving between apps.

This project is a clean native implementation. It does not include AutoHotkey or code copied from `alt-ime-ahk`.

## Behavior

| Input | Behavior |
|---|---|
| Left Alt single tap | IME OFF after `DoubleTapMs` |
| Right Alt single tap | IME ON after `DoubleTapMs` |
| Left Alt tap, then Left Alt down within timeout | Cancel IME OFF and enter normal Left Alt mode |
| Right Alt tap, then Right Alt down within timeout | Cancel IME ON and enter normal Right Alt mode |
| Left Alt + another key | Normal Left Alt shortcut |
| Right Alt + another key | Normal Right Alt shortcut |
| Right Alt held, then Left Alt | Standalone Left Alt |
| Left Alt held, then Right Alt | Standalone Right Alt |

The second same-key Alt press within the timeout is treated as an explicit request for normal Alt behavior. If the second press is released immediately, it behaves like standalone Alt. If another key is pressed while holding it, it behaves like a normal Alt shortcut.

## Settings

Settings are loaded from:

```text
%APPDATA%\ImeKeysForUS\settings.ini
```

Example:

```ini
[Keyboard]
DoubleTapMs=100
```

The default is `100 ms`, clamped to `100-500 ms`. Restart the app after editing settings in v0.1.x.

## Start at Sign-in

MSI installation does not enable startup automatically.

To enable startup for the current user, launch the app, open the tray menu, and enable `Start at sign-in`.

Alternatively:

```powershell
"C:\Program Files\ImeKeysForUS\ime-keys-for-us.exe" --enable-startup
```

To disable:

```powershell
"C:\Program Files\ImeKeysForUS\ime-keys-for-us.exe" --disable-startup
```

## Code signing

The v0.1.2 distribution `exe` and `MSI` are Authenticode-signed with a local `CN=tsuchim` code-signing certificate.

This is a personal open-source project and the certificate is not a public CA-trusted code-signing certificate. Windows SmartScreen or publisher warnings may still appear depending on the environment.

The signature is provided so users can verify that the distributed artifacts have not been modified after signing. If/when the app is distributed through winget, the installer is also verified by the SHA256 hash in the manifest. Winget submission/publication is manual and may not be available at all times.

## Known Limitations

- IME control uses IMM first and may not work perfectly with every IME/application combination.
- TSF support is not implemented yet.
- Elevated application support requires future UIAccess signed-build validation.
- CI artifacts may be unsigned. The preferred v0.1.2 distribution artifacts are locally signed.
- Microsoft Store distribution is out of scope for v0.1.x.
- Right Alt may behave differently on layouts that treat it as AltGr.
- There is no settings UI yet.

## Developer Documentation

Build, signing, UIAccess, and release instructions are in [docs/development.md](docs/development.md).

Manual test checklist: [docs/manual-test-checklist.md](docs/manual-test-checklist.md)

## License

MIT License. See [LICENSE](LICENSE).
