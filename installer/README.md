# Installer

v0.1.2 uses WiX-based installer packaging. The installer is expected to:

- Install to `C:\Program Files\ImeKeysForUS\`.
- Install `ime-keys-for-us.exe`.
- Add a Start Menu shortcut.
- Add uninstall support.
- Support signed executable and signed installer artifacts.

The installer may require elevation because it writes under Program Files.

Do not create a Windows service. The keyboard hook and IME operations belong to the interactive user desktop session.

## WiX

WiX sources are under:

```text
installer/wix/
```

If WiX is unavailable in CI, CI should continue producing the executable artifact and skip installer packaging.

## Startup

v0.1.x does not register auto-start from the MSI by default. This avoids mixing a per-machine Program Files install with an elevated MSI writing to the installing user's `HKCU\Software\Microsoft\Windows\CurrentVersion\Run`.

Startup is controlled by the app for the current interactive user. Use the tray menu item `Start at sign-in`, or run:

```powershell
"C:\Program Files\ImeKeysForUS\ime-keys-for-us.exe" --enable-startup
"C:\Program Files\ImeKeysForUS\ime-keys-for-us.exe" --disable-startup
```

Manual startup options:

- Put a shortcut to `C:\Program Files\ImeKeysForUS\ime-keys-for-us.exe` in the current user's Startup folder.
- Create a current-user Task Scheduler task when early startup or UIAccess/elevated-window testing is needed.

Future versions may add an explicit, user-visible startup option.
