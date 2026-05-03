# SignPath Foundation Application Notes

## Project

- Name: IME Keys for US
- Repository: https://github.com/tsuchim/ime-keys-for-us
- License: MIT
- Latest GitHub Release: v0.1.6
- Distribution:
  - GitHub Releases
  - winget package: `tsuchim.ImeKeysForUS`

## What the App Does

IME Keys for US is a small native Windows tray utility for users who type Japanese with a Japanese IME on a US keyboard.

It maps standalone Left Alt and Right Alt taps to explicit IME OFF and IME ON requests while preserving normal Alt shortcuts.

The app does not show a main window. It runs in the notification area / system tray and provides a tray menu for startup registration and exit.

## Current Release State

The project is already released publicly. GitHub Releases currently provide v0.1.6.

Current artifacts are locally Authenticode-signed with:

```text
Subject: CN=tsuchim
```

This is not a public CA-trusted certificate and is used as an interim integrity measure.

## Privacy

- No telemetry.
- No keystroke content recording.
- No network communication.
- Startup diagnostics remain local.

The low-level keyboard hook is used only to detect Left Alt / Right Alt gestures locally and request IME ON/OFF behavior.

## Why SignPath Foundation

Public CA-trusted code signing is expensive for a small personal open-source project.

SignPath Foundation signing would allow the project to publish signed artifacts through a verifiable open-source build process based on GitHub Actions and the public repository.

## Intended Integration

SignPath Foundation signing is intended for a future release after the application is accepted and the signing workflow is integrated. The intended future release flow is:

1. A signed tag is pushed.
2. GitHub Actions builds unsigned MSI artifacts on GitHub-hosted Windows runners.
3. The unsigned MSI is uploaded as a GitHub Actions artifact.
4. The official SignPath GitHub Action submits a signing request.
5. SignPath verifies the origin metadata.
6. The signed MSI is published to GitHub Releases.
7. The winget manifest references the signed MSI and SHA256.

## SignPath Readiness Checklist

- Public GitHub repository.
- MIT license.
- Public release exists in the form that should be signed.
- winget distribution is available.
- README describes the purpose, tray behavior, diagnostics, and privacy model.
- Code signing policy is published in `docs/code-signing-policy.md` and linked from the README files.
- No telemetry.
- No network communication.
- No proprietary code.
- GitHub Actions release flow exists and can be adapted to SignPath artifact submission after acceptance.
