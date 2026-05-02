# winget

winget submission is prepared but not automatic.

Package identifier:

```text
tsuchim.ImeKeysForUS
```

The final manifest must be generated after the GitHub Release exists because winget requires a stable installer URL and SHA256 hash.

References:

- Manifest authoring: https://learn.microsoft.com/en-us/windows/package-manager/package/manifest
- Repository submission: https://learn.microsoft.com/en-us/windows/package-manager/package/repository
- WinGetCreate update command: https://github.com/microsoft/winget-create

## Flow

1. Complete the project release first.
2. Confirm the GitHub Release has the intended signed MSI attached.
3. Use the stable MSI URL from the GitHub Release.
4. Compute the signed MSI SHA256.
5. Generate the versioned winget manifest using `wingetcreate update tsuchim.ImeKeysForUS -u <installer-url> -v <version>` or update the three YAML files manually.
6. Ensure the manifest uses the current accepted schema and `ManifestVersion`.
7. Validate with `winget validate <manifest-folder>`.
8. Fork or branch `microsoft/winget-pkgs`.
9. Add the manifest under `manifests/t/tsuchim/ImeKeysForUS/<version>/`.
10. Submit a PR to `microsoft/winget-pkgs`.
11. Wait for automated validation and moderator review.
12. Address validation failures or review comments one at a time.

Do not submit the winget PR before the GitHub Release is complete. A winget submission should point at immutable release artifacts, not local, draft, or unsigned CI artifacts.
