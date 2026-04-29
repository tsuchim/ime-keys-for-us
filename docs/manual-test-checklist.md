# Manual Test Checklist

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

- [ ] No settings file uses default `200`.
- [ ] Missing `DoubleTapMs` uses default `200`.
- [ ] Invalid `DoubleTapMs` uses default `200`.
- [ ] `DoubleTapMs=100` works.
- [ ] `DoubleTapMs=500` works.
- [ ] Values below `100` clamp to `100`.
- [ ] Values above `500` clamp to `500`.
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

## UIAccess / Elevation

- [ ] Normal build behavior with non-elevated apps.
- [ ] Behavior over elevated apps.
- [ ] Signed UIAccess build from Program Files.
- [ ] Failure mode when unsigned or installed outside Program Files.
