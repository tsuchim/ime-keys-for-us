# Development

This document collects developer-facing information for IME Keys for US.

ユーザー向け情報は `README.md` にまとめています。このファイルはビルド、署名、UIAccess、CI、リリース作業用です。

## Build

Requirements:

- Windows 10 or 11
- Visual Studio 2022 Build Tools
- CMake 3.21 or later

Build x64 Release:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Release builds link the MSVC runtime statically. This keeps the app self-contained for direct MSI installation and winget review environments.

Expected executable:

```text
build\Release\ime-keys-for-us.exe
```

Depending on the generator layout, the executable may be under a configuration subdirectory below `build`.

## Installer

WiX v4.0.6 is used in CI.

Local MSI build example:

```powershell
build\wix4\wix.exe build installer\wix\Product.wxs -arch x64 -d SourceDir=build\Release -d ProductVersion=0.1.4 -o build\IME-Keys-for-US-0.1.4-x64.msi
```

This example produces an unsigned MSI unless `build\Release\ime-keys-for-us.exe` has already been signed. Sign the executable first when preparing a signed public MSI.

The MSI must:

- Install to `C:\Program Files\ImeKeysForUS\`.
- Add a Start Menu shortcut.
- Support uninstall.
- Not register startup automatically.
- Not create a Windows service.

Startup registration belongs to the app under the current user, not the per-machine MSI.

## Startup CLI

```powershell
.\build\Release\ime-keys-for-us.exe --enable-startup
.\build\Release\ime-keys-for-us.exe --disable-startup
```

These commands write only to:

```text
HKCU\Software\Microsoft\Windows\CurrentVersion\Run
```

Value name:

```text
ImeKeysForUS
```

Value data:

```text
"<current exe path>" --startup
```

## Local Signing

Local signing is optional. The build supports signing with `signtool` by certificate thumbprint:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DIME_KEYS_SIGN_AFTER_BUILD=ON
cmake --build build --config Release
```

Expected local certificate:

- Subject: `CN=tsuchim`
- Thumbprint: `A8B29BB0F03E4955B62E1CA798B6041F29812385`
- Store: `CurrentUser\My`
- EKU: Code Signing

Do not commit certificates, private keys, PFX files, certificate passwords, or secrets.

## UIAccess

Normal unsigned builds run at medium integrity and may not work over elevated administrator windows.

UIAccess-capable builds require:

- Authenticode signing.
- A trusted signing certificate on the local machine.
- Installation in a secure location such as `C:\Program Files\ImeKeysForUS\`.
- A manifest with `uiAccess="true"`.

Configure a UIAccess manifest build with:

```powershell
cmake -S . -B build-uiaccess -G "Visual Studio 17 2022" -A x64 -DIME_KEYS_UIACCESS=ON -DIME_KEYS_SIGN_AFTER_BUILD=ON
cmake --build build-uiaccess --config Release
```

Running the app itself as administrator is not the preferred long-term design. UIAccess is preferred when elevated-window support is needed.

## CI

PR build runs on `windows-latest` and verifies:

- CMake configure.
- x64 Release build.
- startup registration smoke test.
- WiX v4.0.6 install.
- MSI build.
- silent MSI install/uninstall smoke test.
- exe and MSI artifact upload.

GitHub-hosted CI does not test keyboard/IME/tray-click/UIAccess integration. Those remain manual because they depend on an interactive desktop, installed IMEs, shell notification area behavior, low-level keyboard hooks, signing, and elevated-window behavior.

## Release

Current preferred public version:

```text
0.1.4
```

Release tag:

```text
v0.1.4
```

GitHub Release title:

```text
IME Keys for US v0.1.4
```

Before packaging or uninstall testing, disable current-user startup if it was enabled:

```powershell
"C:\Program Files\ImeKeysForUS\ime-keys-for-us.exe" --disable-startup
```

Release process:

1. Run the automated Release Preflight section in `docs/manual-test-checklist.md`.
2. Wait for the PR build to run the CI installer smoke test.
3. Produce locally signed artifacts for the preferred public release.
4. Compute SHA256 hashes.
5. Draft GitHub Release with known limitations.
6. Attach installer.
7. Attach executable zip if useful.
8. Review before publishing.

v0.1.4 distribution artifacts are signed with the local personal `CN=tsuchim` Authenticode certificate. This is not a public CA-trusted code-signing certificate.

See `docs/release-workflow.md` for the end-to-end release branch, PR, review, tag, GitHub Release, and winget update flow.

Do not publish automatically without review.
