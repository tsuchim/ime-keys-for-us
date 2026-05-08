# Keyboard State Machine

The double-tap timeout setting is loaded at startup:

```cpp
DEFAULT_ALT_DOUBLE_TAP_MS = 100
```

The user-facing setting is:

```ini
[Keyboard]
DoubleTapMs=100
```

## States

- `Idle`: no physical Alt gesture is currently held.
- `AltDownHeld`: a physical Alt down was suppressed and is waiting for classification.
- `PendingTap`: Alt was released without another key, and the app is waiting for a possible second same-key tap.
- `NormalShortcut`: another non-Alt key was pressed, so the held Alt down was replayed and the shortcut is allowed to proceed.
- `CrossFallback`: the opposite Alt key emitted standalone Alt for the second key.

`PendingTap` is represented separately from the physical key-down gesture because the first tap keeps a double-tap cancel window open. IME state is changed speculatively after the standalone Alt key is released and is committed or restored when the gesture is classified.

## Event Flow

1. Physical Left Alt or Right Alt down is suppressed initially. The hook records the target foreground window.
2. If a non-Alt key is pressed while Alt is held, the speculative IME request is restored, the held Alt down is replayed with scan-code based `SendInput`, the non-Alt key passes through, and pending tap interpretation for that held gesture is canceled.
3. If the opposite Alt is pressed while one Alt is held, the speculative IME request is restored, standalone Alt for the second key is emitted, both physical Alt events are consumed for that gesture, and IME state is otherwise left to the target app.
4. If Alt up arrives while the gesture is `AltDownHeld`, the hook posts a speculative IME request for the recorded target window and the tap becomes `PendingTap`.
5. If a second same-key Alt down arrives before `DoubleTapMs`, the speculative IME request for the first tap is restored, that Alt down is replayed, and the gesture enters normal Alt mode.
6. If the replayed second Alt is released immediately, Windows sees standalone Alt. If another key is pressed while it is held, Windows sees a normal Alt shortcut.
7. If `PendingTap` expires, the speculative IME request is committed and no restore is attempted.

If an opposite Alt gesture starts while a pending tap exists, an already-expired pending tap is resolved first. A still-active pending tap is canceled and the new physical gesture is treated independently. This avoids a delayed IME side effect while another Alt gesture is starting.

The app uses an event-driven timer for pending taps. The timer is armed only while a pending tap exists and is killed when the pending tap is resolved or canceled.

Synthetic input is marked with `dwExtraInfo` and ignored by the hook to prevent recursion.

The hook does not call IMM APIs directly. `App::HandleMessage()` performs IME control from the normal message loop after receiving the posted request.
