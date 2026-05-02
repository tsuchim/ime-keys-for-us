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

## Upgrade Detection

winget correlates installed Win32 applications with manifests using Add/Remove Programs metadata. Microsoft recommends matching `PackageName` and `Publisher` to the Add/Remove Programs entry, and including MSI product codes in the manifest for the best upgrade experience.

The installer manifest must include `AppsAndFeaturesEntries`:

```yaml
AppsAndFeaturesEntries:
  - DisplayName: IME Keys for US
    Publisher: tsuchim
    DisplayVersion: X.Y.Z
    ProductCode: "{PRODUCT-CODE-FROM-MSI}"
    UpgradeCode: "{C79D216B-8E33-45BF-976B-F52825D3A8E6}"
    InstallerType: wix
```

`ProductCode` changes for each MSI package. Read it from the exact signed MSI that will be submitted:

```powershell
scripts\Get-MsiMetadata.ps1 -MsiPath path\to\IME-Keys-for-US-X.Y.Z-x64.msi
```

For v0.1.4 unsigned local test builds, the MSI metadata was:

```text
ProductCode: {7C271C2B-0F18-452B-BB99-D6D9D6B4AC71}
UpgradeCode: {C79D216B-8E33-45BF-976B-F52825D3A8E6}
```

If winget lists an upgrade but a UI wrapper fails with `0x8A150014` / "No installed package found matching input", prefer a new package version with the explicit `AppsAndFeaturesEntries` metadata. Do not rely on a UI wrapper's cached package mapping as proof that `winget update --id` can re-identify the installed package.
## Flow

1. Complete the project release first.
2. Confirm the GitHub Release has the intended signed MSI attached.
3. Use the stable MSI URL from the GitHub Release.
4. Compute the signed MSI SHA256.
5. Read the MSI metadata with `scripts\Get-MsiMetadata.ps1`.
6. Generate the versioned winget manifest using `wingetcreate update tsuchim.ImeKeysForUS -u <installer-url> -v <version>` or update the three YAML files manually.
7. Ensure the manifest uses the current accepted schema and `ManifestVersion`.
8. Ensure `AppsAndFeaturesEntries` contains the exact MSI `ProductCode` and stable `UpgradeCode`.
9. Validate with `winget validate <manifest-folder>`.
10. Fork or branch `microsoft/winget-pkgs`.
11. Add the manifest under `manifests/t/tsuchim/ImeKeysForUS/<version>/`.
12. Submit a PR to `microsoft/winget-pkgs`.
13. Wait for automated validation and moderator review.
14. Address validation failures or review comments one at a time.

Do not submit the winget PR before the GitHub Release is complete. A winget submission should point at immutable release artifacts, not local, draft, or unsigned CI artifacts.
