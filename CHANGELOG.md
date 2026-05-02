# Changelog

## 0.1.4

- Restored the tray icon when the Windows shell recreates the taskbar.
- Made re-launching an already running instance request a tray icon refresh instead of silently exiting.
- Added release preflight checks and documented the main-to-release-to-winget flow.

## 0.1.3

- Linked the MSVC runtime statically for release builds.
- Added startup diagnostics under `%LOCALAPPDATA%\ImeKeysForUS\ime-keys-for-us.log`.
- Added visible startup error messages for fatal initialization failures and tray icon add failures.
- Clarified README guidance for tray behavior and systems without a Japanese IME.

## 0.1.2

- Fixed the MSI package version to follow the release version passed to WiX.
- Clarified winget wording while publication is still under review.
- Prepared the release for locally signed public artifacts.

## 0.1.1

- Published locally Authenticode-signed distribution artifacts.
- Documented that the signing certificate is a personal `CN=tsuchim` code-signing certificate, not a public CA-trusted certificate.
- Added GitHub funding metadata.

## 0.1.0

- Added native Win32 application skeleton.
- Added low-level keyboard hook for explicit Left Alt IME OFF and Right Alt IME ON single taps after a double-tap timeout.
- Preserved normal Alt shortcuts by replaying the initially suppressed Alt down event.
- Added same-key second Alt press fallback that cancels pending IME and enters normal Alt mode.
- Added cross Alt fallback.
- Added INI-based `DoubleTapMs` setting.
- Added IMM-based IME open status controller.
- Added tray icon with Exit menu.
- Added application icon resource and tray icon from `assets/ImeKeysForUS.png`.
- Added application name header to the tray menu.
- Added current-user `Start at sign-in` tray toggle and CLI options.
- Added single-instance guard.
- Added CMake build with optional signing and UIAccess manifest variant.
- Added installer, winget, release, signing, UIAccess, and manual testing documentation.
