# Keyboard State Machine

Timing constants are defined in `src/keyboard_hook.h`:

```cpp
ALT_LONG_PRESS_MS = 300
ALT_TAP_MAX_MS = 250
```

`ALT_TAP_MAX_MS` is documented for classification and future behavior, but v0.1.0 does not implement double-tap behavior.

## States

- `Idle`: no pending Alt gesture.
- `Undecided`: a physical Alt down was suppressed and is waiting for classification.
- `NormalShortcut`: another non-Alt key was pressed, so the held Alt down was replayed and the shortcut is allowed to proceed.
- `StandaloneFallback`: long press emitted a standalone Alt down/up.
- `CrossFallback`: the opposite Alt key emitted standalone Alt for the second key.

## Event Flow

1. Physical Left Alt or Right Alt down is suppressed initially.
2. If a non-Alt key is pressed while Alt is held, the held Alt down is replayed with `SendInput`, the non-Alt key passes through, and IME state is unchanged.
3. If the long-press threshold is reached first, standalone Alt down/up is emitted and IME state is unchanged.
4. If the opposite Alt is pressed, standalone Alt for the second key is emitted and IME state is unchanged.
5. If Alt up arrives while the gesture is undecided, Left Alt sets IME OFF and Right Alt sets IME ON.

Synthetic input is marked with `dwExtraInfo` and ignored by the hook to prevent recursion.

