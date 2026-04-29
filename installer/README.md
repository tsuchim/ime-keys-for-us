# Installer

v0.1.0 prepares WiX-based installer packaging. The installer is expected to:

- Install to `C:\Program Files\ImeKeysForUS\`.
- Install `ime-keys-for-us.exe`.
- Add a Start Menu shortcut.
- Support current-user logon startup registration.
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

Preferred startup registration is current-user logon startup from the installer. Users who need early startup or UIAccess/elevated-window testing can create a Task Scheduler task manually.

