# IME Keys for US

IME Keys for US is a small native Windows utility for Japanese users on US keyboards.

It maps Left Alt and Right Alt to explicit IME OFF and IME ON behavior. It avoids IME toggle semantics so users do not need to remember the current IME state while moving between apps.

This project is a clean native implementation. It does not include AutoHotkey or code copied from `alt-ime-ahk`.

## What this app does

IME Keys for US is a small Windows tray utility for people who type Japanese on a US keyboard.

It is not a text editor or a keyboard layout. It does not show a main window. After launch, it runs in the notification area / system tray.

The goal is to avoid IME toggle behavior and provide explicit IME ON / OFF keys:

- Left Alt: IME OFF / alphanumeric input
- Right Alt: IME ON / Japanese input

When moving between apps and text fields, users often need to remember the current IME state. This utility reduces that burden by making English and Japanese input modes explicit.

## What appears after launch

When the app starts normally, an icon appears in the Windows notification area / system tray.

No main window is shown.

Right-clicking the tray icon opens a menu with:

- Start at sign-in
- Exit

If the icon is not visible, also check the hidden notification icons area.

However, if no tray icon appears and Alt key presses have no effect, that is not expected. Starting with v0.1.3, startup diagnostics are written to:

```text
%LOCALAPPDATA%\ImeKeysForUS\ime-keys-for-us.log
```

Fatal initialization failures also show a message box.

## Behavior on systems without a Japanese IME

The main function of this app is to explicitly switch a Japanese IME ON and OFF.

On systems where a Japanese IME is not installed or not active, pressing Left Alt or Right Alt may not produce an obvious visible change.

Even in that case, the tray icon should still appear if the app started successfully.

If the tray icon does not appear, treat it as a startup issue rather than an IME-state issue.

## Example usage check

On a system with a Japanese IME, such as Microsoft IME for Japanese:

1. Launch IME Keys for US.
2. Confirm that the tray icon appears.
3. Open a text field, such as Notepad.
4. Make sure a Japanese IME is available.
5. Press Right Alt and confirm that Japanese input is enabled.
6. Press Left Alt and confirm that alphanumeric input is enabled.
7. Right-click the tray icon and choose Exit to quit the app.

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

## Privacy

IME Keys for US does not collect telemetry, does not record keystroke contents, and does not send data to any network service.

The low-level keyboard hook is used only to detect Left Alt / Right Alt gestures locally and request IME ON/OFF behavior.

Startup diagnostics are written only to the local machine:

```text
%LOCALAPPDATA%\ImeKeysForUS\ime-keys-for-us.log
```

This log is not uploaded automatically.

## Code signing

The release distribution `exe` and `MSI` artifacts are Authenticode-signed during release with a local `CN=tsuchim` code-signing certificate.

This is a personal open-source project and the certificate is not a public CA-trusted code-signing certificate. Windows SmartScreen or publisher warnings may still appear depending on the environment.

The signature is provided so users can verify that the distributed artifacts have not been modified after signing. In winget, the installer is also verified by the SHA256 hash in the manifest.

The project intends to apply for SignPath Foundation open-source code signing. If accepted, SignPath Foundation signing is planned for releases `0.1.5` and later.

Code signing policy: [docs/code-signing-policy.md](docs/code-signing-policy.md)

## Known Limitations

- IME control uses IMM first and may not work perfectly with every IME/application combination.
- TSF support is not implemented yet.
- Elevated application support requires future UIAccess signed-build validation.
- CI artifacts may be unsigned. The preferred v0.1.4 distribution artifacts are locally signed.
- Microsoft Store distribution is out of scope for v0.1.x.
- Right Alt may behave differently on layouts that treat it as AltGr.
- There is no settings UI yet.

## Developer Documentation

Build, signing, UIAccess, and release instructions are in [docs/development.md](docs/development.md).

Manual test checklist: [docs/manual-test-checklist.md](docs/manual-test-checklist.md)

## License

MIT License. See [LICENSE](LICENSE).
