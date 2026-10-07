/bin/bash: line 1: s3nt1n3lz21/EnderalBookMarker: No such file or directory
/bin/bash: line 1: s3nt1n3lz21/enderal-book-markers: No such file or directory
/bin/bash: line 1: EnderalBookMarker: command not found
/bin/bash: line 1: s3nt1n3lz21: command not found
/bin/bash: line 1: s3nt1n3lz21/enderal-book-markers: No such file or directory
# Enderal Manual Book Marker Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a Vortex-installable Enderal SE mod that manually toggles a persistent personal-read marker on books and displays a separate inventory icon beside SkyUI's built-in read icon.

**Architecture:** A CommonLibSSE-NG SKSE plugin resolves the selected/open book, toggles marker state in the SKSE co-save, and registers a custom condition with DIII. DIII's rule adds an exported SWF icon without a `replace` field, so the native `readIcon` is left intact. The selected-item path and icon placement still need in-game verification.

**Tech Stack:** Enderal SE 2.0.12.4 / expected Skyrim 1.5.97, SKSE, CommonLibSSE-NG, SkyUI, Dynamic Inventory Icon Injector (DIII), C++ and Python build/test tools, Vortex package.

**Spec:** `docs/superpowers/specs/2026-10-07-enderal-manual-book-marker-design.md`

## Global Constraints

- Target: Enderal SE, installed and managed with Vortex.
- The player can toggle the marker with one configurable hotkey while a book is selected in inventory or while its reading screen is open.
- Accidentally opening a book must not set the personal marker.
- The marker must be reversible and persist across game sessions.
- The inventory marker must be visually separate and to the right of the existing read icon. Do not rename items or alter the built-in read icon.
- Markers are keyed by the book's base form, so all copies of the same book share one personal read state.
- Nexus Mods packaging/release comes after in-game testing; no public release is implied by creating the repository.

## Review Focus

- Runtime mismatch: confirm the native plugin loads on the actual Enderal runtime before testing any in-game feature.
- No selected/open book: hotkey must leave state unchanged.
- Unresolvable book or malformed save data: fail safely without a crash or unintended marker.
- Same book in inventory and open view: both routes must toggle the same base-form marker.
- UI overlap/reskin: show the marker beside the native read icon without hiding it or changing item names/search/sorting.

---

### Task 1: Verify target runtime and modding route

**Files:**
- Create: `docs/compatibility/enderal-se-runtime.md` (record exact local Enderal/SKSE versions and selected plugin framework)

**Interfaces:**
- Consumes: agreed user experience from the design spec.
- Produces: a verified runtime target and a short compatibility record that selects the SKSE framework and inventory icon path before coding.

- [ ] **Step 1: Record installed versions**

Read the user's Enderal SE runtime version, Enderal version, SKSE version, and SkyUI version from their local installation or logs. Do not update or replace their game installation as part of this check.

- [ ] **Step 2: Verify candidate frameworks against the recorded runtime**

Check supported runtime versions for the SKSE plugin framework, save serialization mechanism, hotkey/menu input mechanism, and candidate icon injector. Select an injector only if it supports conditional per-book custom icons and can place a new status icon beside the existing read status without replacing it.

- [ ] **Step 3: Write the compatibility decision**

Record exact version identifiers, dependency versions, source links, and any unsupported UI reskins in `docs/compatibility/enderal-se-runtime.md`. If the required plugin or icon route cannot support the installed runtime, stop and present the constraint and revised route before implementation.

- [ ] **Step 4: Review compatibility gate**

Confirm that both inventory selection and open-book context can be resolved, marker state can be serialized per save, and the distinct icon can be displayed. Expected: all three are feasible on the recorded target; otherwise revise the spec and plan before proceeding.

### Task 2: Create the source repository and build skeleton

**Files:**
- Existing GitHub repository `s3nt1n3lz21/enderal-book-markers`
- Create: `README.md`
- Create: `.gitignore`
- Create: `CMakeLists.txt` or the build manifest selected in Task 1
- Create: source and test directories matching the selected SKSE framework
- Create: `.github/workflows/build.yml`

**Interfaces:**
- Consumes: Task 1 compatibility record.
- Produces: a buildable repository with documented runtime target and CI artifact output.

- [x] **Step 1: Use the existing GitHub repository**

The user created the public `enderal-book-markers` repository under `s3nt1n3lz21`. Source work is on `dev/compatibility-and-plugin-skeleton`; the default `main` branch has not received the implementation.

- [x] **Step 2: Add the plugin entry point and build configuration**

Added a CommonLibSSE-NG plugin entry point, Windows CMake target, and vcpkg manifest. Startup logging is still to be added before a release build.

- [x] **Step 3: Add build verification**

CI runs Linux unit/icon checks and attempts the Windows CommonLibSSE-NG build. After the build succeeds, it stages a test package and uploads it as an artifact. Generated SWF files and DLLs are kept out of source control.

- [ ] **Step 4: Build locally and in CI**

Run the documented local build and the GitHub Actions workflow. Expected: both produce the plugin DLL with no missing runtime dependency.

- [x] **Step 5: Commit repository skeleton**

Commit the buildable skeleton and compatibility record before adding behavior.

### Task 3: Implement manual marker state and hotkey toggling

**Files:**
- Create: `src/BookMarkerState.*`
- Create: `src/BookContext.*`
- Create: `src/HotkeyHandler.*`
- Create: `tests/BookMarkerStateTests.*`
- Modify: plugin registration and configuration files

**Interfaces:**
- Consumes: runtime/plugin setup from Task 2.
- Produces: `BookContext::GetCurrentBook() -> std::optional<FormID>` and `BookMarkerState::Toggle(FormID) -> bool`, where the returned value is `true` when marked and `false` when unmarked.

- [x] **Step 1: Write marker state tests**

Test first toggle adds a form ID; second toggle removes it; another form remains independent; serialization round-trip restores the same set; empty or invalid serialized input yields an empty set.

- [x] **Step 2: Run tests and confirm they fail**

Run the repository's C++ test target. Expected: tests fail because marker storage/toggle behavior is not implemented.

- [x] **Step 3: Implement marker set and SKSE co-save serialization**

Store unique base FormIDs using the save serialization mechanism selected in Task 1. Store only mod-owned data; do not mutate the game's built-in book read flag.

- [x] **Step 4: Implement book context resolution**

Resolve a supported book from the currently selected inventory item, or from the currently open book menu. Prefer the open-book context when that menu is active. Ignore non-book forms and absent context.

- [ ] **Step 5: Implement configurable hotkey and feedback (prototype currently uses fixed F6)**

Register one configurable default hotkey, allow rebinding through the selected configuration mechanism, and display a brief marked/unmarked confirmation. If no book is selected/open, do nothing. If context resolution fails, leave marker state unchanged.

- [ ] **Step 6: Run tests and review changes (Linux tests pass; Windows plugin build is pending)**

Run unit tests and build. Expected: all marker-state tests pass and the plugin loads without changing game read state.

### Task 4: Display the custom marker in the inventory

**Files:**
- Create: `Data/SKSE/Plugins/<selected-injector-config>` or the focused SkyUI interface extension selected in Task 1
- Create: `Data/Interface/<custom-icon-asset>` if required
- Modify: plugin-to-icon state integration
- Create: `tests/InventoryMarkerIntegrationTests.*` if the selected framework has testable rules

**Interfaces:**
- Consumes: `BookMarkerState::IsMarked(FormID) -> bool` from Task 3.
- Produces: inventory UI displaying the custom icon only for personally marked book forms, positioned immediately to the right of the original read icon.

- [x] **Step 1: Define the custom icon asset**

Use a distinct, legible monochrome marker icon that does not reuse or replace SkyUI's `readIcon`. Add its source asset and license/attribution information to the repository.

- [x] **Step 2: Connect per-book marker state to the UI**

Use the selected injector's supported registration/config interface or the focused SkyUI extension to display the icon only when the book's personal marker state is set. Preserve the vanilla read indicator and all item-name text.

- [ ] **Step 3: Verify UI state combinations**

Check marked/unmarked crossed with vanilla read/unread. Expected: the personal marker appears only when marked, and the normal read icon continues to reflect the game's own state.

- [ ] **Step 4: Verify search and sorting**

In game, search for a marked book by its original title and sort by name. Expected: same search result and title-based sort behavior as without the mod.

### Task 5: Package for Vortex and test in Enderal

**Files:**
- Create: `dist/Vortex/` staging layout
- Create: `README.md` installation, configuration, compatibility, and removal instructions
- Create: `.github/workflows/release.yml` or packaging script

**Interfaces:**
- Consumes: tested plugin and UI files from Tasks 2–4.
- Produces: a versioned archive Vortex can install and a reproducible build/package process.

- [ ] **Step 1: Add Vortex-ready package layout**

Include the plugin, icon config/assets, and only the required scripts/interface files. Do not bundle third-party frameworks; document and declare them as dependencies.

- [ ] **Step 2: Test installation and removal through Vortex**

Install and enable the archive, launch Enderal through its SKSE loader, verify the plugin log, then disable/uninstall through Vortex. Expected: the mod installs cleanly and removal does not leave loose files or alter saves beyond its own serialized data.

- [ ] **Step 3: Run the in-game acceptance checks**

Verify toggling on/off from inventory and open-book view, accidental opening alone, save/reload persistence, same-title copies sharing one marker, no-book input, unknown-book failure, skill-book behavior, icon placement/read-state combinations, and title search/sorting.

- [ ] **Step 4: Produce a test archive and release notes**

Create a clearly labeled test archive for the user to install. Do not upload to Nexus Mods until the user has tested it and asked for release preparation.

### Task 6: Final verification and release readiness

**Files:**
- Modify: `README.md` and release notes with verified compatibility and test results

**Interfaces:**
- Consumes: Vortex-installed build and acceptance results from Task 5.
- Produces: a reviewed, reproducible source repository and a decision-ready Nexus release package.

- [ ] **Step 1: Run clean build and all automated tests**

Run local build, unit tests, and GitHub Actions on the final commit. Expected: all pass for the declared runtime.

- [ ] **Step 2: Review the final diff and package contents**

Confirm there are no game binaries, save files, secrets, unrelated assets, or unsupported runtime claims in the repository/archive.

- [ ] **Step 3: Record tested versions and known limits**

Update README and release notes with exact Enderal, Skyrim runtime, SKSE, SkyUI, injector, Vortex, and UI-reskin versions tested.

- [ ] **Step 4: Present release package for user testing**

Provide the source repository and test archive. Nexus publication remains a separate user-directed action after their in-game test.
