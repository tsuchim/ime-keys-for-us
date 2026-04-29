# Release

Current preferred public version:

```text
0.1.1
```

Release tag:

```text
v0.1.1
```

GitHub Release title:

```text
IME Keys for US v0.1.1
```

## Process

1. Build x64 Release.
2. Run manual tests from `docs/manual-test-checklist.md`.
3. Produce locally signed artifacts for the preferred public release.
4. Compute SHA256 hashes.
5. Draft GitHub Release with known limitations.
6. Attach installer.
7. Attach executable zip if useful.
8. Review before publishing.

v0.1.1 distribution artifacts are signed with the local personal `CN=tsuchim` Authenticode certificate. This is not a public CA-trusted code-signing certificate.

Before packaging or uninstall testing, disable current-user startup if it was enabled:

```powershell
"C:\Program Files\ImeKeysForUS\ime-keys-for-us.exe" --disable-startup
```

Do not publish automatically without review.

## PR Body Template

```markdown
## Summary

## Implemented behavior

## Build

## Signing

## Installer

## Manual test checklist

## Known limitations

## Release notes draft
```
