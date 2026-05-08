# Outlook Search IME Experiments

Date: 2026-05-04

This note records experiments for the issue where IME Keys for US works in many
normal input fields but does not switch IME state in the top search box of new
Outlook.

## Baseline

The released implementation primarily uses IMM open-status control:

1. Capture the foreground window.
2. Resolve its default IME window with `ImmGetDefaultIMEWnd`.
3. Send `WM_IME_CONTROL` with `IMC_SETOPENSTATUS`.

Observed behavior:

- Works in many normal input fields.
- Does not affect the top search box in new Outlook.
- In the Outlook search box, IMM state checks can report success or matching
  state even though the user-visible input state does not change.

This suggests that the Outlook search box uses an input context that is not
controlled correctly by the external IMM default IME window path, or that the
state observed through IMM is not the effective state of the actual search box.

## Experiment 1: Speculative set on Alt down

Change:

- Request IME ON/OFF immediately on Alt down.
- Remember the prior state for best-effort restore if the gesture becomes a
  double tap or normal Alt shortcut.
- Use `SendMessageTimeoutW` for IMM window messages to avoid hanging the app
  thread if the target IME window is unresponsive.

Result:

- The approach is implementable.
- It does not address the Outlook search box problem by itself because the
  underlying set method is still the same IMM open-status control.
- It also introduced additional complexity around restore ordering and target
  validation.

Conclusion:

- Useful as a responsiveness design, but not a fix for Outlook search.

## Experiment 2: Synthetic `VK_IME_ON` / `VK_IME_OFF`

Hypothesis:

- New Outlook search accepts keyboard-path IME operations, so official IME
  virtual keys might work better than external IMM control.

Test changes:

- Added temporary commands:
  - `--send-vk-ime-on`
  - `--send-vk-ime-off`
- Sent virtual-key based `SendInput` down/up events with
  `SyntheticInputGuard::Marker()`.
- Also tried using this path as the primary set method for Alt handling.

Result:

- `SendInput` returned success.
- The app logged successful sends.
- In practice, IME did not switch reliably, and at one point did not switch
  anywhere through the Alt path.

Conclusion:

- `VK_IME_ON` / `VK_IME_OFF` is not a working primary method in this environment.
- It should not replace IMM as the default behavior based on current evidence.

## Experiment 3: Sending IME keys after Alt release

Hypothesis:

- `VK_IME_ON` / `VK_IME_OFF` might fail because they are sent while physical Alt
  is still held.

Test change:

- Record the Alt gesture on key down.
- Send the IME virtual key only after the standalone Alt key is released.
- Keep the double-tap timeout only as a cancellation/restore window.

Result:

- The executable was rebuilt and relaunched from `build\Release`.
- `SendInput` still reported success.
- IME still did not switch through the Alt path.

Conclusion:

- The failure is not only caused by sending while Alt is physically down.

## Experiment 4: Direct `Shift+Space`

Hypothesis:

- The user observed that `Shift+Space` toggles IME in the Outlook search box.
- If Alt sends `Shift+Space`, Outlook search should respond.

Test change:

- Temporarily made Alt send synthetic `Shift+Space` through `SendInput`.

Result:

- Outlook search box responded.
- Normal input fields also responded.
- However, both Left Alt and Right Alt became IME toggle keys.

Conclusion:

- Keyboard-path synthetic `Shift+Space` reaches the Outlook search box.
- This is not acceptable as the general behavior because it breaks the app's
  core explicit semantics:
  - Left Alt must mean IME OFF.
  - Right Alt must mean IME ON.

## Experiment 5: Hybrid IMM plus `Shift+Space` fallback

Hypothesis:

- Use IMM for normal explicit ON/OFF.
- Use `Shift+Space` only when IMM cannot be verified.

Test change:

- Try IMM first.
- If the pre-state is known and differs from the requested state, and IMM does
  not verify, send `Shift+Space`.
- If state cannot be known, avoid `Shift+Space` to prevent unintended toggles.

Result:

- Normal input fields worked.
- Outlook search box still did not work.
- Logs showed that Outlook search could report `IMM IME set verified` or
  `IME already matched requested state`, causing fallback not to run even though
  the visible search-box IME state did not change.

Conclusion:

- IMM verification is not reliable for the Outlook search box.
- The hybrid fallback cannot decide correctly based only on IMM state.

## Experiment 6: Force `Shift+Space` for Outlook process targets

Hypothesis:

- Since `Shift+Space` works in Outlook search, force it for Outlook processes.

Test change:

- Detect target process image path containing:
  - `olk.exe`
  - `outlook.exe`
- For those targets, bypass IMM and send `Shift+Space`.

Result:

- Outlook search box toggled IME from Alt.
- This proves that the Outlook target can be reached through the keyboard input
  path.
- It still does not provide explicit ON/OFF. It makes Alt an IME toggle in
  Outlook.

Conclusion:

- This is a working technical fallback for Outlook search, but it is not a good
  product behavior for this app because it violates explicit ON/OFF semantics.

## Current conclusion

The Outlook search box problem is not solved cleanly yet.

Known facts:

- IMM open-status control works for many normal targets.
- IMM open-status control is unreliable for the new Outlook top search box.
- `VK_IME_ON` / `VK_IME_OFF` did not work as a practical replacement in this
  environment.
- Synthetic `Shift+Space` reaches the Outlook search box and toggles IME there.
- `Shift+Space` is not an acceptable general replacement because it is a toggle.
- IMM state checks cannot be trusted to infer the visible Outlook search-box IME
  state.

The forced Outlook `Shift+Space` fallback is useful as a diagnostic proof, but
it is rejected as product behavior. It must not be implemented as the current
specification because it changes Alt from explicit ON/OFF into a toggle in
Outlook and therefore violates the app's core semantics.

## Decision: reject `Shift+Space` fallback

All currently considered `Shift+Space` fallback variants are rejected for the
current specification:

- Always sending `Shift+Space`.
- Sending `Shift+Space` only when IMM verification fails.
- Forcing `Shift+Space` for Outlook process targets.
- Keeping `--send-shift-space` as an implementation-facing test command.

The knowledge remains useful: it proves that Outlook search can receive
keyboard-path IME operations. However, because `Shift+Space` is a toggle, it is
not acceptable as a feature implementation for a tool whose primary contract is
explicit Left Alt = IME OFF and Right Alt = IME ON.

## Decision: reject `VK_IME_ON` / `VK_IME_OFF`

Synthetic `VK_IME_ON` / `VK_IME_OFF` is also rejected for the current
specification.

The idea was attractive because these virtual keys represent explicit IME ON and
OFF rather than a toggle. In practice, `SendInput` reported success but the
environment did not reliably switch IME state. It did not solve the Outlook
search-box issue and was worse than the existing IMM path in normal use.

The knowledge remains in this file, but the implementation must not keep
`VK_IME_ON` / `VK_IME_OFF` as the primary path, fallback path, or command-line
test surface unless a future investigation identifies conditions where it works
reliably.

## Open directions

Possible next investigations:

1. Find a TSF-based explicit open/close method for the actual focused context.
2. Investigate whether the Outlook search box exposes a different focused child
   window, UI Automation element, WebView2 host, or thread input context than the
   foreground window used by the current IMM path.
3. Determine whether Microsoft IME has a reliable keyboard-path explicit ON/OFF
   sequence other than `VK_IME_ON` / `VK_IME_OFF` in this environment.
4. Do not revisit `Shift+Space` fallback unless the product requirement changes
   explicitly to allow a toggle-only compatibility mode.

## Related local commits

Experimental commits on `codex/speculative-ime-alt`:

```text
e0bb3a6 Start IME changes on Alt down
d717b93 Add VK_IME_ON_OFF test commands
7fda226 Use VK_IME_ON_OFF as primary IME set path
5f2e74c Allow speculative IME set without restore state
6217843 Send IME keys after standalone Alt release
6b2bd37 Restore IMM primary IME set path
1ff8228 Add Shift Space fallback for unverified IME set
45c8388 Force Shift Space fallback for Outlook targets
```

At the time of this note, the currently running diagnostic build was launched
from:

```text
C:\Users\tsuchim\Github\ime-keys-for-us\build\Release\ime-keys-for-us.exe
```
