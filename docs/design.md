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
- Same-key second Alt press fallback for normal Alt behavior.
- Cross Alt fallback.
- Tray icon with Exit.
- Tray menu current-user startup toggle.
- Single-instance guard.
- INI-based double-tap timeout setting.

## Event Dispatch

The low-level keyboard hook only detects keyboard gestures and performs the minimum input replay needed to preserve Alt shortcuts and fallback behavior.

IME ON/OFF is not executed directly from the hook callback. When an Alt tap timeout is classified as an IME operation, `KeyboardHook` posts one of these custom messages to the hidden app window:

- `WM_APP_SET_IME_OFF`
- `WM_APP_SET_IME_ON`

`App::HandleMessage()` receives that message on the normal Win32 message loop and calls `ImeController::SetOpenStatus()`. This keeps synchronous IMM work, including `SendMessageW(... WM_IME_CONTROL, ...)`, out of the low-level keyboard hook path.

## Double-tap Fallback

Double-tap is implemented by treating the second same-key Alt press within the configured timeout as an explicit request for normal Alt behavior. The first tap does not change IME immediately. If the timeout expires without a second same-key Alt down, the pending tap becomes an IME request.

When the second same-key Alt down arrives within the timeout, the pending IME request is canceled and that second Alt down is replayed as a normal Alt down. If the user releases it immediately, Windows sees standalone Alt. If the user presses another key while holding it, Windows sees a normal Alt shortcut. This intentionally avoids guessing ambiguous intent after the second press.

If an opposite Alt gesture starts while a pending tap exists, an already-expired pending tap is resolved first. A still-active pending tap is canceled and the new physical gesture is treated independently. This avoids a delayed IME side effect while the user is starting another Alt gesture.

Settings are loaded once at startup from:

```text
%APPDATA%\ImeKeysForUS\settings.ini
```

```ini
[Keyboard]
DoubleTapMs=100
```

The default is `100 ms`. `DoubleTapMs` is loaded as a positive integer number of milliseconds. `0`, negative values, non-numeric values, and values too large to handle safely use the default. Restart is required after editing settings in v0.1.x.

## Startup Order

The startup order is:

1. Create the hidden message window.
2. Install the keyboard hook.
3. Add the tray icon.

The tray icon is useful for Exit, but tray icon creation failure is not fatal. If the shell notification area is temporarily unavailable, the keyboard hook can still run.

The double-tap timer is not a permanent polling timer. The hook posts `WM_APP_KEYBOARD_PENDING_CHANGED` when a pending tap starts or clears, and the app message loop arms a timer only while a pending tap exists.

## Startup Registration

The MSI does not register automatic startup. Startup is controlled by the app for the current interactive user.

The tray menu includes `Start at sign-in`, which toggles:

```text
HKCU\Software\Microsoft\Windows\CurrentVersion\Run
```

Value name:

```text
ImeKeysForUS
```

Value data:

```text
"<current exe path>" --startup
```

The app also supports `--enable-startup` and `--disable-startup` command-line options. These commands write only to `HKEY_CURRENT_USER` and exit immediately.

Out of scope for v0.1.0:

- General hotkey configuration.
- Settings UI.
- Long-press standalone Alt fallback.
- TSF.
- GitHub-hosted CI keyboard, IME, tray-click, and UIAccess integration tests.
- Updater.
- Telemetry.
- Store/MSIX distribution.
