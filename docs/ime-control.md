# IME Control

v0.1.x originally used the Windows IMM approach:

1. Get the foreground window with `GetForegroundWindow`.
2. Get the default IME window with `ImmGetDefaultIMEWnd`.
3. Send `WM_IME_CONTROL` with `IMC_SETOPENSTATUS`.
4. Use `0` for OFF and `1` for ON.

The current implementation uses the IMM open-status path. Synthetic
`VK_IME_ON` / `VK_IME_OFF` keyboard input was tested and rejected as a product
implementation because it did not switch IME reliably in the test environment.

IME control is isolated in:

```text
src/ime_controller.h
src/ime_controller.cpp
```

The low-level keyboard hook does not call `ImeController` directly. It posts an app-defined message to the hidden window, and the normal Win32 message loop calls `ImeController::SetOpenStatus()`.

## Immediate Alt-release IME control

The app may request IME ON/OFF as soon as a physical standalone Alt tap is
released, before the double-tap timeout expires. This favors visible IME
responsiveness over perfectly avoiding temporary IME state changes.

The implementation must keep these constraints:

1. The low-level keyboard hook records only the gesture id, Alt side, and target
   foreground `HWND`, then posts a message to the hidden window and returns.
2. The hook callback must not call IMM APIs or synchronous window messaging.
3. The hidden window / app thread performs best-effort get, set, and restore.
4. Restore is attempted only for a gesture whose speculative set recorded the
   original open status successfully.
5. Restore validates that the target `HWND` still refers to the same thread and
   process observed when the speculative set ran.
6. Composition state, candidate windows, IME internal state, and the fact that
   the target app may observe an IME open-status change are not restored.

This design is expected to affect only the target application's IME/input
state. It should not steal focus, launch another application, or require
elevation. The remaining practical risks are:

- A target app may observe temporary IME ON/OFF transitions.
- Composition or candidate UI can be affected by the temporary open-status
  change.
- If the target app or IME window is hung, synchronous IME control can stall the
  app thread briefly when the IMM fallback is used. IME window messages should
  therefore use `SendMessageTimeoutW` with a short timeout.
- UIPI can block IME control messages to elevated target apps. In that case
  speculative set or restore may fail, which is an existing limitation of
  non-UIAccess builds.

## TSF

TSF is not implemented in v0.1.0. A future version may add TSF-based IME state control if IMM-based control is insufficient for some IMEs or applications.
