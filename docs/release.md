# Release

Initial version:

```text
0.1.0
```

Release tag:

```text
v0.1.0
```

GitHub Release title:

```text
IME Keys for US v0.1.0
```

## Process

1. Build x64 Release.
2. Run manual tests from `docs/manual-test-checklist.md`.
3. Produce unsigned CI artifacts or locally signed artifacts.
4. Compute SHA256 hashes.
5. Draft GitHub Release with known limitations.
6. Attach installer.
7. Attach executable zip if useful.
8. Review before publishing.

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
