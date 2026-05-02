# winget Packaging

The winget package identifier is:

```text
tsuchim.ImeKeysForUS
```

The template under `tsuchim.ImeKeysForUS/template/` is incomplete until the GitHub Release URL, installer SHA256, and MSI `ProductCode` are known.

The winget manifest must include `AppsAndFeaturesEntries` with the MSI `ProductCode` and stable `UpgradeCode` so winget can reliably correlate the installed Add/Remove Programs entry with `tsuchim.ImeKeysForUS` during upgrade.
