# Design

IME Keys for US is a narrow native Win32 utility. It maps thumb-accessible US keyboard Alt keys to explicit Japanese IME state:

- Left Alt tap: English / IME OFF
- Right Alt tap: Japanese / IME ON

The app intentionally avoids toggle behavior. The user should not need to remember the current IME mode of the foreground application.

## Scope

In scope for v0.1.0:

- Low-level keyboard hook.
- Explicit IME ON/OFF by single Alt tap after the double-tap timeout.
- Normal Alt shortcut preservation.
- Same-key double-tap standalone Alt fallback.
- Cross Alt fallback.
- Tray icon with Exit.
- Single-instance guard.
- INI-based double-tap timeout setting.

## Event Dispatch

The low-level keyboard hook only detects keyboard gestures and performs the minimum input replay needed to preserve Alt shortcuts and fallback behavior.

IME ON/OFF is not executed directly from the hook callback. When an Alt tap timeout is classified as an IME operation, `KeyboardHook` posts one of these custom messages to the hidden app window:

- `WM_APP_SET_IME_OFF`
- `WM_APP_SET_IME_ON`

`App::HandleMessage()` receives that message on the normal Win32 message loop and calls `ImeController::SetOpenStatus()`. This keeps synchronous IMM work, including `SendMessageW(... WM_IME_CONTROL, ...)`, out of the low-level keyboard hook path.

## Double-tap Fallback

Standalone Alt is emitted by double-tapping the same Alt key within the configured timeout. The first tap does not change IME immediately. If the timeout expires without a second same-key tap, the pending tap becomes an IME request.

If an opposite Alt gesture starts while a pending tap exists, the pending tap is canceled and the new physical gesture is treated independently. This avoids a delayed IME side effect while the user is starting another Alt gesture.

Settings are loaded once at startup from:

```text
%APPDATA%\ImeKeysForUS\settings.ini
```

```ini
[Keyboard]
DoubleTapMs=200
```

The default is `200 ms`, clamped to `100-500 ms`. Restart is required after editing settings in v0.1.0.

## Startup Order

The startup order is:

1. Create the hidden message window.
2. Install the keyboard hook.
3. Start the keyboard timer that resolves pending double-tap timeouts.
4. Add the tray icon.

The tray icon is useful for Exit, but tray icon creation failure is not fatal. If the shell notification area is temporarily unavailable, the keyboard hook can still run.

Out of scope for v0.1.0:

- General hotkey configuration.
- Settings UI.
- Long-press standalone Alt fallback.
- TSF.
- Updater.
- Telemetry.
- Store/MSIX distribution.
