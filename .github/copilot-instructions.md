# Copilot Instructions

## Project Scope

IME Keys for US is a narrow native Win32 tray utility for explicit Japanese IME ON/OFF behavior on US keyboards.

- Preserve explicit IME ON/OFF semantics.
- Do not turn the project into a general hotkey manager.
- Keep IME control isolated in `src/ime_controller.*`.
- Keep the low-level keyboard hook callback fast.
- Avoid unnecessary dependencies.

## Release Review Focus

For release PRs, review the release process as well as the code.

- Flag release PRs that update versions but describe a release as already published before the GitHub Release exists.
- Flag release PRs that imply SignPath signing is active before the SignPath workflow is actually integrated.
- Check that release notes, README files, installer docs, and `CHANGELOG.md` agree on the intended release version.
- Check that release PRs do not rely on CI alone when review comments are still unresolved.
- Check that a canceled or skipped version is documented clearly and that the next public version is unambiguous.

## winget Review Focus

For winget manifest PRs:

- Prefer the no-suffix signed MSI asset for `InstallerUrl`.
- Do not point winget to unsigned CI artifacts.
- Require `AppsAndFeaturesEntries` metadata when MSI upgrade correlation is relevant.
- Ensure the MSI `ProductCode` is present where the manifest schema expects it.
- Avoid redundant `AppsAndFeaturesEntries.InstallerType` when it matches the root installer type.
- Avoid introducing version-correlation fields that conflict with older published manifests unless the older manifests are backfilled or the field is intentionally omitted.
