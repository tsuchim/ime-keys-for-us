# Agent Instructions

- Keep the project narrow.
- Do not turn this into a general hotkey manager.
- Preserve explicit IME ON/OFF semantics.
- Do not implement IME toggle as the primary behavior.
- Do not copy AutoHotkey scripts, GPL implementations, or line-by-line translations of existing IME scripts.
- Keep the keyboard hook callback fast.
- Keep IME control isolated in `src/ime_controller.*`.
- Keep signing material out of the repo.
- Prefer a native Win32 implementation.
- Avoid unnecessary dependencies.
- Require manual testing for keyboard behavior.
- Treat release, tag, push, merge, and publication actions as guarded actions.

## Release and Publication Guardrails

These rules are mandatory for any agent preparing, merging, tagging, releasing, or submitting winget updates.

- Do not treat passing CI as release approval.
- Do not merge a release PR until GitHub code review has completed.
- A GitHub code review is not complete merely because no review is visible immediately after PR creation. Wait for the asynchronous review window and re-check `latestReviews` and `reviewThreads`.
- Before merging a release PR, verify and report all of the following:
  - PR checks are passing.
  - `latestReviews` includes the expected GitHub code review or a human maintainer review for the current PR state.
  - Every review thread is resolved.
  - Every review comment has been classified as accepted, rejected with a reason, or addressed with an alternative fix.
- Do not create or push a release tag until the merged release PR satisfied the review gate above.
- Do not create or update GitHub Release artifacts until the release tag points to a commit that passed the review gate.
- Do not submit a winget PR until the GitHub Release contains the final signed MSI URL and SHA256.
- Do not leave winget PR review threads unresolved. Address every Copilot, bot, validation, and moderator comment before considering the winget submission ready.
- If a review comment appears after an accidental merge or tag:
  - Stop publication immediately.
  - Cancel any in-progress release workflow if release artifacts are not final.
  - Do not move an already-pushed public tag unless the user explicitly approves tag deletion/recreation.
  - Prefer a follow-up PR and the next patch version.

## Required GitHub Review Checks

Use thread-aware checks before guarded actions. At minimum, inspect:

```powershell
gh pr view <number> --json latestReviews,reviewDecision,mergeStateStatus,statusCheckRollup,url
gh api graphql -F owner=<owner> -F name=<repo> -F number=<number> -f query='query($owner:String!,$name:String!,$number:Int!){ repository(owner:$owner,name:$name){ pullRequest(number:$number){ reviewThreads(first:100){ nodes{ id isResolved isOutdated path line comments(first:10){ nodes{ author{login} body createdAt } } } } } } }'
```

For `microsoft/winget-pkgs` PRs, run the same thread-aware review check against `owner=microsoft` and `name=winget-pkgs`.
