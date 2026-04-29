# IME Control

v0.1.0 uses the Windows IMM approach:

1. Get the foreground window with `GetForegroundWindow`.
2. Get the default IME window with `ImmGetDefaultIMEWnd`.
3. Send `WM_IME_CONTROL` with `IMC_SETOPENSTATUS`.
4. Use `0` for OFF and `1` for ON.

IME control is isolated in:

```text
src/ime_controller.h
src/ime_controller.cpp
```

The low-level keyboard hook does not call `ImeController` directly. It posts an app-defined message to the hidden window, and the normal Win32 message loop calls `ImeController::SetOpenStatus()`.

## TSF

TSF is not implemented in v0.1.0. A future version may add TSF-based IME state control if IMM-based control is insufficient for some IMEs or applications.
