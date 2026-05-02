# winget Template

This template is intentionally incomplete.

Fill in after the GitHub Release exists:

- Installer URL
- Installer SHA256
- Release date if needed
- MSI `ProductCode`

The `AppsAndFeaturesEntries` block is required for reliable upgrade detection.
Read the `ProductCode` from the exact MSI that will be submitted to winget.
The `UpgradeCode` should remain `{C79D216B-8E33-45BF-976B-F52825D3A8E6}` unless `installer/wix/Product.wxs` intentionally changes.
