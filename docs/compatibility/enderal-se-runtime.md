# Enderal SE runtime compatibility

## Target

- **Enderal SE:** 2.0.12.4, identified by the user.
- **Expected Skyrim runtime:** 1.5.97. Steam maps its Enderal 2.0.12.4 package to Skyrim SE 1.5.97. The user's screenshot showed the default public branch and installed content last updated September 9, 2026, before the announced Steam migration to Enderal 2.1.4.4 / Skyrim 1.7.104.
- **Local executable confirmation:** pending. The game installation is not available in this workspace, so the expected runtime is based on the selected Enderal branch/version rather than a direct read of `SkyrimSE.exe`.
- **SKSE target:** 2.0.20 is the official SKSE build for runtime 1.5.97. The installed SKSE version has not been confirmed.
- **Plugin framework:** CommonLibSSE-NG, targeting pre-AE Skyrim 1.5.x; SKSE co-save serialization for marker data.

## Prototype inventory icon route: DIII

Use Dynamic Inventory Icon Injector (DIII) for the prototype. It adds status icons, supports multiple matching icons, and provides an SKSE plugin API for custom per-entry match conditions. Its `ICondition::Match` callback receives the inventory entry, so the marker plugin can check the entry's base form against its personal-read set. Our rule will add an icon without setting DIII's optional `replace: "readIcon"` field, preserving the built-in read indicator.

The DIII download page does not itself clearly list 1.5.97 compatibility. However, the maintainer of a separate DIII integration reported testing on Skyrim 1.5.97 and published logs showing registration of the DIII plugin listener. This is useful compatibility evidence, but not a substitute for testing the current DIII version with Enderal.

## Context and persistence route

- Open book: CommonLib exposes `BookMenu::GetTargetForm()`; a public SKSE plugin uses it to get the form currently displayed in the book menu.
- Inventory selection: SkyUI's `InventoryMenu` exposes the selected entry's `formId` through its Scaleform object. The implementation can read the selected entry from the active menu's movie.
- Hotkey: the current prototype registers an SKSE input event sink after `kInputLoaded` and uses F6 (keyboard scan code `0x40`). User-configurable hotkey settings are not implemented yet.
- Save data: SKSE's serialization interface provides save/load/revert callbacks and form-ID resolution for co-save data.

The repository now contains a native plugin prototype for the hotkey toggle and SKSE co-save persistence, plus Linux-tested state/context code. The selected-entry Scaleform path and all runtime callbacks still need a successful Windows build and in-game verification. The DIII rule and custom icon asset are not implemented yet.

## Remaining gates

1. Confirm the installed `SkyrimSE.exe`, SKSE, and SkyUI versions when available.
2. Build and load the plugin and current DIII release on the Enderal 2.0.12.4 installation.
3. Verify the custom icon appears beside the native read icon and neither icon replaces the other.
4. Test inventory selection and open-book hotkey routes, co-save persistence, and any UI reskin conflicts.

## Sources

- [Steam: Enderal SE update announcement](https://store.steampowered.com/app/976620/Enderal_Forgotten_Stories/) — maps 2.0.12.4 to Skyrim 1.5.97 and 2.1.4.4 to Skyrim 1.7.104.
- [SKSE official site](https://skse.silverlock.org/) — lists SKSE 2.0.20 for game version 1.5.97.
- [CommonLibSSE-NG](https://github.com/CharmedBaryon/CommonLibSSE-NG) — supports pre-AE 1.5.x builds and exposes the SKSE interfaces used for serialization and plugin loading.
- [DIII configuration and plugin API](https://github.com/JerryYOJ/Dynamic-Inventory-Icon-Injector-SKSE/blob/master/CONFIGURATION.md) — documents status icons, custom match conditions, and optional vanilla-icon replacement.
- [DIII at Nexus Mods](https://www.nexusmods.com/skyrimspecialedition/mods/174136) — documents its status-icon function and dependencies.
- [Known Spell Tomes Icon source](https://github.com/cbeaulieu-gt/skyrim_known_spells_icon) — example DIII condition integration with declared Skyrim 1.5.x support.
- [Known Spell Tomes Icon posts](https://www.nexusmods.com/skyrimspecialedition/mods/174651?tab=posts) — author notes testing on 1.5.97 and includes a DIII listener-registration log.
- [SkyUI source](https://github.com/schlangster/skyui/blob/master/src/ItemMenus/ItemMenu.as) — selected inventory entries used by the implementation.
- [SKSE serialization interface](https://github.com/CharmedBaryon/CommonLibSSE-NG/blob/main/include/SKSE/Interfaces.h) — save/load/revert callbacks and form-ID resolution.
- [BookMenu target form example](https://github.com/Sacralletius/ANDR_SKSEFunctions/blob/main/plugin.cpp) — retrieves the currently open book form.
