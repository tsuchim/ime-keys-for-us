# Changelog

## 0.1.0

- Added native Win32 application skeleton.
- Added low-level keyboard hook for explicit Left Alt IME OFF and Right Alt IME ON single taps after a double-tap timeout.
- Preserved normal Alt shortcuts by replaying the initially suppressed Alt down event.
- Added same-key double-tap standalone Alt fallback.
- Added cross Alt fallback.
- Added INI-based `DoubleTapMs` setting.
- Added IMM-based IME open status controller.
- Added tray icon with Exit menu.
- Added single-instance guard.
- Added CMake build with optional signing and UIAccess manifest variant.
- Added installer, winget, release, signing, UIAccess, and manual testing documentation.
