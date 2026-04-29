# Signing and UIAccess

UIAccess is the preferred long-term path for supporting elevated administrator windows. Running the app itself as administrator is not the preferred design.

## Normal Build

Normal builds use `uiAccess="false"` and do not require administrator elevation.

Limitations:

- Unsigned medium-integrity builds may not work in elevated administrator windows.
- Behavior over administrator-launched applications must be tested manually.

## UIAccess Build

UIAccess-capable builds require:

- Authenticode signing.
- A signing certificate trusted on the local machine.
- Installation in a secure location such as `C:\Program Files\ImeKeysForUS\`.
- A manifest with `uiAccess="true"`.

Build example:

```powershell
cmake -S . -B build-uiaccess -G "Visual Studio 17 2022" -A x64 -DIME_KEYS_UIACCESS=ON -DIME_KEYS_SIGN_AFTER_BUILD=ON
cmake --build build-uiaccess --config Release
```

Expected install path:

```text
C:\Program Files\ImeKeysForUS\ime-keys-for-us.exe
```

## Local Certificate

Use the existing local code signing certificate when building signed local artifacts:

- Subject: `CN=tsuchim`
- Thumbprint: `A8B29BB0F03E4955B62E1CA798B6041F29812385`
- Store: `CurrentUser\My`
- EKU: Code Signing

Example:

```powershell
signtool sign `
  /sha1 A8B29BB0F03E4955B62E1CA798B6041F29812385 `
  /fd SHA256 `
  /tr http://timestamp.digicert.com `
  /td SHA256 `
  <file>
```

Do not commit certificates, private keys, PFX files, certificate passwords, or secrets.

