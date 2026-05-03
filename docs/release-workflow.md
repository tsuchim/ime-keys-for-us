# Release Workflow

This document defines the project release flow from version selection through the follow-up winget submission.

## Version Selection

Use semantic versioning:

- Patch: bug fixes, diagnostics, packaging fixes, and documentation for the current behavior.
- Minor: new user-visible behavior, new supported workflow, or meaningful installer/startup behavior changes.
- Major: incompatible behavior or a deliberate change to core IME key semantics.

For a bug fix after `0.1.5`, the next release is `0.1.6`.

## Main PR

1. Decide the next release number.
2. Update project version metadata.
3. Update `CHANGELOG.md`.
4. Update release documentation and user-facing version references.
5. Run the automated Release Preflight section in `docs/manual-test-checklist.md`.
   The PR build runs the installer smoke test in GitHub Actions. Do not trigger UAC elevation from the agent.
6. Create a PR targeting `main`.
7. Leave the PR open for GitHub code review. Do not merge after CI alone.

## Review Handling

1. Wait for GitHub code review to complete.
   Passing CI is not review completion.
   If `latestReviews` is empty immediately after PR creation, wait and check again.
2. Inspect each review comment independently.
3. Classify each comment as accepted, rejected, or requiring a different change.
4. For accepted comments, make the smallest suitable code or documentation change.
5. For rejected comments, explain the reason in the PR thread.
6. For comments that indicate a real issue but propose the wrong fix, implement the better fix and explain it.
7. Run the relevant tests after each meaningful change set.
8. Resolve review threads only after the fix or response is complete.
9. Re-check review state after every push.
10. When every issue is resolved and checks pass, merge normally into `main`.

Do not squash unrelated cleanup into the release PR after review starts unless it directly supports a review finding.

Before merge, record the review gate evidence in the work log or final report:

```powershell
gh pr view <PR_NUMBER> --json latestReviews,reviewDecision,mergeStateStatus,statusCheckRollup,url
gh api graphql -F owner=tsuchim -F name=ime-keys-for-us -F number=<PR_NUMBER> -f query='query($owner:String!,$name:String!,$number:Int!){ repository(owner:$owner,name:$name){ pullRequest(number:$number){ reviewThreads(first:100){ nodes{ id isResolved isOutdated path line comments(first:10){ nodes{ author{login} body createdAt } } } } } } }'
```

The merge gate is closed unless:

- PR checks pass.
- GitHub code review has appeared in `latestReviews` or a human maintainer has reviewed the current PR.
- Every `reviewThreads.nodes[*].isResolved` value is `true`.
- All accepted comments have corresponding commits or explanation comments.

Do not create a release tag while this gate is closed.

## Tag And GitHub Release

1. Sync local `main` after the PR is merged.
2. Confirm that the merged PR passed the Review Handling gate above.
3. Create a signed annotated tag:

```powershell
git tag -s vX.Y.Z -m "IME Keys for US vX.Y.Z"
```

4. Push the tag:

```powershell
git push origin vX.Y.Z
```

5. Wait for the GitHub Release workflow to finish.
6. Replace or supplement unsigned CI artifacts with the intended signed public artifacts when preparing the public release.
7. Confirm release notes, artifacts, signatures, and SHA256 hashes before publishing or announcing.

If a review comment appears after a mistaken merge or tag push, stop publication immediately. Cancel any in-progress release workflow when final artifacts are not ready. Do not move an already-pushed public tag without explicit user approval; prefer a follow-up PR and the next patch version.

## winget Update

Submit the winget update after the GitHub Release is complete.

The Windows Package Manager repository submission flow requires validating the manifest and submitting a PR to `microsoft/winget-pkgs`:

- https://learn.microsoft.com/en-us/windows/package-manager/package/repository
- https://learn.microsoft.com/en-us/windows/package-manager/package/manifest

WinGetCreate supports updating existing manifests and can submit PRs:

- https://github.com/microsoft/winget-create

Use this project flow:

1. Confirm the GitHub Release has the signed MSI that should be installed by winget.
2. Copy the stable GitHub Release MSI URL.
3. Compute the signed MSI SHA256.
4. Read the MSI `ProductCode` and `UpgradeCode`:

```powershell
scripts\Get-MsiMetadata.ps1 -MsiPath path\to\IME-Keys-for-US-X.Y.Z-x64.msi
```

5. Generate or update the winget manifest for `tsuchim.ImeKeysForUS`.
6. Include `AppsAndFeaturesEntries` with the exact MSI `ProductCode` and stable `UpgradeCode`.
7. Validate locally:

```powershell
winget validate <manifest-folder>
```

8. Add the manifest to `microsoft/winget-pkgs` under:

```text
manifests/t/tsuchim/ImeKeysForUS/X.Y.Z/
```

9. Open a PR to `microsoft/winget-pkgs`.
10. Wait for automated validation and moderator review.
11. Address each validation failure or review comment individually.
12. Resolve all review threads after the fix or response is complete.
13. Re-check that every review thread is resolved before reporting the winget PR as ready.

Do not file the winget PR before the GitHub Release exists and the signed MSI URL and hash are final.

For winget PRs, use the same thread-aware review check:

```powershell
gh pr view <PR_NUMBER> --repo microsoft/winget-pkgs --json latestReviews,reviews,mergeStateStatus,statusCheckRollup,url
gh api graphql -F owner=microsoft -F name=winget-pkgs -F number=<PR_NUMBER> -f query='query($owner:String!,$name:String!,$number:Int!){ repository(owner:$owner,name:$name){ pullRequest(number:$number){ reviewThreads(first:100){ nodes{ id isResolved isOutdated path line comments(first:10){ nodes{ author{login} body createdAt } } } } } } }'
```
