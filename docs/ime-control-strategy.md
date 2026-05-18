# IME Control Strategy

## Current Baseline

IME Keys for US uses the Windows IMM path for v0.1.x:

1. Capture the foreground window at the Alt gesture boundary.
2. Resolve the GUI thread's focused control with `GetGUIThreadInfo`.
3. Prefer `GUITHREADINFO.hwndFocus` as the IME target.
4. Fall back to the foreground window when a focused child cannot be resolved.
5. Call `ImmGetDefaultIMEWnd` for the resolved target.
6. Send `WM_IME_CONTROL` with `IMC_SETOPENSTATUS`.

This keeps the v0.1.7 latency behavior: standalone Left/Right Alt release
requests IME OFF/ON immediately. `DoubleTapMs` is only the cancellation /
restore window for same-key double tap behavior.

## Prior Art And Documentation Basis

`alt-ime-ahk` is useful historical prior art because it resolves the focused
window before sending IME control. This project does not copy AutoHotkey code or
line-by-line implementation details. The project decision is based on Win32/IMM
documentation, the documented `GetGUIThreadInfo` focused-window API, and local
verification in normal controls such as Notepad.

`ImmGetDefaultIMEWnd` and `WM_IME_CONTROL` are documented Win32/IMM APIs. The
operating system creates a default IME window for each thread, and applications
can send IME control messages to that window. `GetGUIThreadInfo` is the
documented way to retrieve GUI-thread state, and `GUITHREADINFO.hwndFocus` is
the window with keyboard focus.

## Open Status Checks

`GetOpenStatus` is useful for capturing restore state before a speculative IME
request. It must not suppress the explicit standalone Alt set request. In
particular, the standalone Alt path must not skip `IMC_SETOPENSTATUS` merely
because an earlier `IMC_GETOPENSTATUS` result appeared to match the requested
state.

The v0.1.8 `known_open` send-skip optimization was removed in v0.1.9 because it
could trust an open-status value from the wrong effective input target and avoid
sending the actual set request.

## TSF

TSF is the modern broader text-services architecture for advanced text input and
language technologies. It remains a plausible future direction for difficult
targets, but adding TSF support is a separate design project. It is not part of
the v0.1.9 patch release because this release is intentionally limited to the
focused-HWND IMM regression fix.

## Outlook Search

v0.1.9 does not claim to solve the new Outlook search-box issue. Existing local
experiments showed that Outlook search can report IMM success or matching state
while the visible IME state does not change. Synthetic `Shift+Space` can reach
that field, but it is a toggle and violates the product contract that Left Alt
means IME OFF and Right Alt means IME ON.

Outlook search remains a known limitation and a future TSF or target-specific
investigation area.
