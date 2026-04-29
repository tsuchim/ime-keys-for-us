# Design

IME Keys for US is a narrow native Win32 utility. It maps thumb-accessible US keyboard Alt keys to explicit Japanese IME state:

- Left Alt tap: English / IME OFF
- Right Alt tap: Japanese / IME ON

The app intentionally avoids toggle behavior. The user should not need to remember the current IME mode of the foreground application.

## Scope

In scope for v0.1.0:

- Low-level keyboard hook.
- Explicit IME ON/OFF by standalone Alt tap.
- Normal Alt shortcut preservation.
- Long-press standalone Alt fallback.
- Cross Alt fallback.
- Tray icon with Exit.
- Single-instance guard.

Out of scope for v0.1.0:

- General hotkey configuration.
- Double-tap behavior.
- TSF.
- Updater.
- Telemetry.
- Store/MSIX distribution.

