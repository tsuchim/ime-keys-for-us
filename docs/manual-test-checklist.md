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

## Key Behavior

- [ ] Left Alt tap sets IME OFF.
- [ ] Right Alt tap sets IME ON.
- [ ] Alt+F still opens menus.
- [ ] Alt+Space still opens window menu.
- [ ] Alt+Tab still works.
- [ ] Alt+Left / Alt+Right still work in browser.
- [ ] Long press Left Alt opens menu as standalone Alt.
- [ ] Long press Right Alt behaves as configured.
- [ ] Right Alt held, then Left Alt emits standalone Left Alt.
- [ ] Left Alt held, then Right Alt emits standalone Right Alt.
- [ ] IME state does not change during normal Alt shortcuts.
- [ ] IME state does not change during long-press fallback.
- [ ] IME state does not change during cross Alt fallback.
- [ ] Synthetic input does not recurse.

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

