# Enderal Book Markers

An SKSE mod for Enderal: Forgotten Stories Special Edition. It lets you manually mark and unmark books as personally read, with a separate inventory icon planned alongside the game's normal read icon.

## Planned behavior

- Toggle the personal marker while a book is selected in inventory or open in the reading menu.
- Opening a book alone will not mark it.
- Toggle again to remove the marker.
- Show a small personal marker beside the game's existing read icon.
- Keep book names, search, sorting, and the game's built-in read status unchanged.
- Save marker state with the game save; copies of the same book share a marker.

## Development status

The repository now has a C++ plugin prototype for a configurable hotkey toggle, save persistence, and DIII marker condition, plus a DIII rule and a source generator for an original bookmark/check icon. The marker-state, hotkey configuration, rule, and generated-SWF structure have automated tests. The Windows plugin build and in-game behavior still need verification.

The target is Enderal SE 2.0.12.4, corresponding to Skyrim runtime 1.5.97. The exact local executable, installed SKSE and SkyUI versions, and in-game behavior still need confirmation.

## Requirements

The plugin build uses CommonLibSSE-NG through vcpkg. Enderal SE, compatible SKSE, SkyUI, and Dynamic Inventory Icon Injector (DIII) are required to run the current icon prototype. Compatibility with the user's Enderal setup remains to be tested.

## Hotkey configuration

The default toggle is F6. Edit `Data/SKSE/Plugins/EnderalBookMarkers.ini` to change its keyboard scan code:

```ini
[Input]
ToggleKey=0x40
```

The value may be decimal or `0x`-prefixed hexadecimal. The ini file is read beside the game's `Data` directory when the plugin loads.

## Build

On Windows with CMake, Visual Studio C++ tools, Git, and vcpkg available:

```powershell
cmake -S . -B build -A x64 -DCMAKE_TOOLCHAIN_FILE="C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build --config Release
```

The manifest and registry configuration in the repository install CommonLibSSE-NG. On Linux, the build configures and runs the standalone state tests:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Installation

The Windows CI job is set to create a test archive for Vortex after the plugin build succeeds. It is not a release; Enderal in-game verification is still required.

## Development references

See the [runtime compatibility notes](docs/compatibility/enderal-se-runtime.md), [design specification](docs/superpowers/specs/2026-10-07-enderal-manual-book-marker-design.md), and [implementation plan](docs/superpowers/plans/2026-10-07-enderal-manual-book-marker.md).

## License

License to be chosen before public release.
