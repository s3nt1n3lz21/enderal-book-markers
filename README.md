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

The repository now has a C++ plugin prototype for the F6 toggle and save persistence, plus unit-tested marker-state and book-context logic. The separate inventory icon is not implemented yet. The prototype still needs a successful Windows build and testing in Enderal before it can be treated as usable.

The target is Enderal SE 2.0.12.4, corresponding to Skyrim runtime 1.5.97. The exact local executable, installed SKSE and SkyUI versions, and in-game behavior still need confirmation.

## Requirements

The plugin build uses CommonLibSSE-NG through vcpkg. Enderal SE and a compatible SKSE installation are required to run it. The planned separate inventory icon uses Dynamic Inventory Icon Injector (DIII); whether it works with the user's Enderal setup remains to be tested.

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

No Vortex-ready release archive is available yet. Vortex packaging and installation instructions will follow after the plugin build and in-game behavior are verified.

## Development references

See the [runtime compatibility notes](docs/compatibility/enderal-se-runtime.md), [design specification](docs/superpowers/specs/2026-10-07-enderal-manual-book-marker-design.md), and [implementation plan](docs/superpowers/plans/2026-10-07-enderal-manual-book-marker.md).

## License

License to be chosen before public release.
