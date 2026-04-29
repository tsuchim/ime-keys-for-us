# winget

winget submission is prepared but not automatic in v0.1.0.

Package identifier:

```text
tsuchim.ImeKeysForUS
```

The final manifest must be generated after the GitHub Release exists because winget requires a stable installer URL and SHA256 hash.

## Flow

1. Create GitHub Release.
2. Download or reference the installer URL.
3. Compute SHA256.
4. Generate winget manifest using `wingetcreate` or manually.
5. Validate with `winget validate`.
6. Fork `microsoft/winget-pkgs`.
7. Add manifest under the correct path.
8. Submit PR to `microsoft/winget-pkgs`.
9. Wait for automated validation and review.

