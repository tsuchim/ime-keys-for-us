# Agent Instructions

- Keep the project narrow.
- Do not turn this into a general hotkey manager.
- Preserve explicit IME ON/OFF semantics.
- Do not implement IME toggle as the primary behavior.
- Do not copy AutoHotkey scripts, GPL implementations, or line-by-line translations of existing IME scripts.
- Keep the keyboard hook callback fast.
- Keep IME control isolated in `src/ime_controller.*`.
- Keep signing material out of the repo.
- Prefer a native Win32 implementation.
- Avoid unnecessary dependencies.
- Require manual testing for keyboard behavior.
- Treat release, tag, push, merge, and publication actions as guarded actions.

