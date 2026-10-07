# Enderal Manual Book Marker Design

## Goal

Create an Enderal: Forgotten Stories Special Edition mod that lets the player manually mark and unmark books as personally read, with a separate marker in the inventory and no changes to book names or Enderal's built-in read status.

## User and constraints

- Target: Enderal SE, installed and managed with Vortex.
- Source lives in the GitHub repository `s3nt1n3lz21/enderal-book-markers`, with possible Nexus Mods release later.
- The player can toggle the marker with one configurable hotkey while a book is selected in inventory or while its reading screen is open.
- Accidentally opening a book must not set the personal marker.
- The marker must be reversible and persist across game sessions.
- The inventory marker must be visually separate and to the right of the existing read icon. Do not rename items or alter the built-in read icon.

## Proposed user experience

The player selects a book in inventory and presses the configured hotkey, or presses it while that book is open. A short message confirms whether the book was marked or unmarked. In inventory, marked books show an additional small icon immediately to the right of SkyUI's existing read icon. Pressing the hotkey again clears the personal marker. Book names, normal title search, sorting, and vanilla read state remain untouched.

Markers are keyed by the book's base form, so all copies of the same book share one personal read state. The first version covers book forms (including notes and skill books when represented as books); marking a skill book does not change its game effect or permit it to be read again.

## Technical approach

Use a small SKSE plugin for capturing the hotkey in both inventory and book-reading menus, identifying the relevant book form, and persisting the player's marker set in save data. Use a separate SkyUI-compatible inventory icon integration to render a custom marker beside—not in place of—the existing read icon.

The exact icon injection mechanism and SKSE runtime compatibility must be verified against Enderal SE before implementation. Prefer an existing supported injector if it can display a custom per-book marker and works with Enderal's runtime. If it cannot, use a focused SkyUI interface extension with documented compatibility limits. Avoid changing SkyUI's item names or built-in `readIcon` state.

## Components

- **Hotkey and book context:** configurable key; resolve the selected inventory entry or currently open book; ignore input when no supported book context exists.
- **Marker state:** toggle state by stable book form identity; serialize it with the save; tolerate empty and older saves.
- **Inventory display:** display the custom marker only for personally marked book forms; preserve the native read indicator, title, sorting, and search behavior.
- **Vortex package and documentation:** include required files, dependency/runtime requirements, installation notes, configuration instructions, and known UI compatibility limits.

## Error handling

If the hotkey is pressed without a book selected/open, do nothing and show no error. If a book form cannot be identified, leave state unchanged and show a brief failure message. Missing or invalid saved marker data should load as an empty set rather than preventing the game from loading.

## Validation

- Confirm runtime and framework compatibility on an Enderal SE installation.
- In game, toggle on/off from inventory and from an open book; confirm both routes target the same book.
- Confirm an accidental open alone adds no marker.
- Confirm marker survives save, exit, and reload, and does not appear on a different book.
- Confirm the custom marker appears to the right of the built-in read icon, while both read and unread states remain distinguishable.
- Confirm book names, title search, and name sorting remain unchanged.
- Confirm skill-book effects are unchanged and the packaged files can be installed/removed through Vortex.

## Release scope

First milestone is a working, locally testable Enderal SE mod and clean source repository. Nexus Mods packaging/release comes after in-game testing; no public release is implied by creating the repository.

## Open implementation check

Before choosing dependencies, verify whether Enderal SE's runtime supports an existing inventory icon injector and SKSE serialization route for per-save custom markers. The Steam screenshot provided shows the default public branch selected but does not establish the exact installed runtime.
