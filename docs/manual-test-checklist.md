# Manual Test Checklist

## Release Preflight Automation

Run the automated preflight script:

```powershell
scripts\Test-ReleasePreflight.ps1 -BuildDir build
```

This verifies:

- [ ] `git diff --check` reports no whitespace errors.
- [ ] CMake configure succeeds.
- [ ] Release build succeeds.
- [ ] Release executable exists.
- [ ] Startup registration smoke test passes.
- [ ] Unsigned x64 MSI builds with WiX.

The PR build runs the same script with `-IncludeInstallerSmoke` on GitHub Actions. That CI-only path also verifies:

- [ ] Silent MSI install succeeds.
- [ ] Installed executable exists at `C:\Program Files\ImeKeysForUS\ime-keys-for-us.exe`.
- [ ] MSI install does not create the current-user startup Run value.
- [ ] Silent MSI uninstall succeeds.
- [ ] Installed executable is removed after uninstall.

Do not trigger UAC elevation from an automated agent run. If the current process is not already elevated, `scripts\Test-Installer.ps1` fails instead of asking for elevation.

## Release Artifact Checks

- [ ] Confirm release artifacts are signed before public release.
- [ ] Compute SHA256 for release artifacts.
- [ ] Confirm winget manifest version, URLs, hashes, schema, and `ManifestVersion` match the release.

## Apps

- [ ] Notepad
- [ ] Microsoft Word
- [ ] VS Code
- [ ] Edge
- [ ] Chrome
- [ ] File Explorer
- [ ] Windows Terminal
- [ ] PowerShell
- [ ] Administrator-launched PowerShell
- [ ] Administrator-launched app

## IMEs

- [ ] Microsoft Japanese IME
- [ ] Google Japanese Input if installed
- [ ] ATOK if installed

## IME Single Tap

- [ ] Left Alt single tap waits briefly, then sets IME OFF.
- [ ] Right Alt single tap waits briefly, then sets IME ON.
- [ ] The delay uses `DoubleTapMs`.

## Double Tap

- [ ] Left Alt tap, then Left Alt down within `DoubleTapMs`, cancels pending IME OFF and enters normal Left Alt mode.
- [ ] Right Alt tap, then Right Alt down within `DoubleTapMs`, cancels pending IME ON and enters normal Right Alt mode.
- [ ] Releasing the second Alt immediately behaves like standalone Alt.
- [ ] Pressing another key while holding the second Alt behaves like normal Alt+key.
- [ ] Double-tap fallback does not change IME state.

## Settings

- [ ] No settings file uses default `100`.
- [ ] Missing `DoubleTapMs` uses default `100`.
- [ ] Invalid `DoubleTapMs` uses default `100`.
- [ ] `DoubleTapMs=0` uses default `100`.
- [ ] Negative `DoubleTapMs` uses default `100`.
- [ ] `DoubleTapMs=10` is accepted.
- [ ] `DoubleTapMs=100` works.
- [ ] `DoubleTapMs=500` works.
- [ ] `DoubleTapMs=1000` is accepted.
- [ ] Values above `2147483647` use default `100`.
- [ ] Restart is required after changing settings.

## Normal Alt Shortcuts

- [ ] Alt+F still opens menus.
- [ ] Alt+Space still opens window menu.
- [ ] Alt+Tab still works.
- [ ] Alt+Left / Alt+Right still work in browser.
- [ ] Alt+Enter still works.
- [ ] Alt+underlined menu accelerator still works.
- [ ] Normal Alt shortcuts do not trigger IME switching.

## Cross Alt

- [ ] Right Alt held, then Left Alt emits standalone Left Alt.
- [ ] Left Alt held, then Right Alt emits standalone Right Alt.
- [ ] IME state does not change during cross Alt fallback.
- [ ] Synthetic input does not recurse.

## Removed Long Press

- [ ] Holding Alt alone does not emit standalone Alt.
- [ ] Holding Alt and then pressing another key still allows normal Alt+key shortcut behavior.

## Startup

- [ ] App starts after logon.
- [ ] Tray icon appears.
- [ ] Exit menu works.
- [ ] Single-instance guard works.
- [ ] Re-launching does not create duplicate hooks.
- [ ] Re-launching an already running app refreshes the existing tray icon instead of silently doing nothing.
- [ ] Re-launching an already running app logs `Requesting existing instance tray refresh.`.
- [ ] Re-launching an already running app logs `Tray icon refreshed.` from the existing instance.
- [ ] Restarting Explorer restores the tray icon via the `TaskbarCreated` notification.
- [ ] After tray restoration, right-clicking the tray icon still opens the menu.
- [ ] After tray restoration, Left Alt / Right Alt IME behavior still works.
- [ ] Fresh MSI install does not create HKCU Run value.
- [ ] App starts manually from Start Menu.
- [ ] Tray menu shows `Start at sign-in` unchecked initially.
- [ ] Clicking `Start at sign-in` creates HKCU Run value.
- [ ] Tray menu shows checked after enabling.
- [ ] Log out/in or restart starts the app automatically.
- [ ] Clicking `Start at sign-in` again removes HKCU Run value.
- [ ] `--enable-startup` creates HKCU Run value.
- [ ] `--disable-startup` removes HKCU Run value.
- [ ] Uninstall removes installed files.
- [ ] If HKCU Run value remains after uninstall, it is harmless but should be removed before uninstall by disabling startup.

## UIAccess / Elevation

- [ ] Normal build behavior with non-elevated apps.
- [ ] Behavior over elevated apps.
- [ ] Signed UIAccess build from Program Files.
- [ ] Failure mode when unsigned or installed outside Program Files.
