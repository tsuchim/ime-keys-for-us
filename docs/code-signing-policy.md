# Code Signing Policy

## Current Status

IME Keys for US currently publishes locally Authenticode-signed release artifacts.

The current local signing certificate is:

```text
Subject: CN=tsuchim
```

This is not a public CA-trusted code-signing certificate. It is used to provide artifact integrity for early public releases.

## Intended Future Status

The project intends to apply for SignPath Foundation open-source code signing.

If accepted, release artifacts should be signed through SignPath.io using SignPath Foundation open-source signing infrastructure.

SignPath Foundation's required notice for accepted open-source projects is:

```text
Free code signing provided by SignPath.io, certificate by SignPath Foundation
```

This notice will apply once the project is accepted and artifacts are signed through SignPath Foundation.

## Release Source

Release artifacts must be built from signed Git tags in this repository.

## Current Release Process

1. Merge the release PR into `main`.
2. Create a signed Git tag, for example `v0.1.4`.
3. Build from the exact tag.
4. Sign the executable and MSI.
5. Upload the signed MSI to the GitHub Release.
6. Update the winget manifest to point to the signed MSI and its SHA256.

## Future SignPath Release Process

SignPath Foundation signing is intended for `0.1.5` and later releases, after the application is accepted and the signing workflow is integrated.

Once SignPath integration is available:

1. GitHub Actions builds unsigned release artifacts from a signed tag.
2. The unsigned MSI is uploaded as a GitHub Actions workflow artifact.
3. SignPath verifies the GitHub Actions origin metadata and artifact.
4. SignPath signs the approved artifacts.
5. Signed artifacts are published to GitHub Releases.
6. The winget manifest points to the SignPath-signed MSI and SHA256.

## Roles

This is currently a single-maintainer project.

| Role | Responsibility | Assignee |
| --- | --- | --- |
| Author | Writes code and prepares changes | @tsuchim |
| Reviewer | Reviews release changes | @tsuchim |
| Approver | Approves release artifacts for signing | @tsuchim |

## Privacy

IME Keys for US does not intentionally collect personal data.

It does not collect telemetry, does not record keystroke contents, and does not send data to any network service.

Startup diagnostics are written only to the local machine under:

```text
%LOCALAPPDATA%\ImeKeysForUS\ime-keys-for-us.log
```

The log is not uploaded automatically.
