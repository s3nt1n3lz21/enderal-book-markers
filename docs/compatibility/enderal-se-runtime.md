# Enderal SE runtime compatibility

## Current target

- **Enderal SE:** 2.0.12.4, as identified by the user.
- **Expected Skyrim runtime:** 1.5.97. Steam describes its 2.0.12.4 package as running Skyrim SE 1.5.97. The user's Steam screenshot showed the default public branch and installed content last updated September 9, 2026, before Steam's announced September 25, 2026 migration to Enderal 2.1.4.4 / Skyrim 1.7.104.
- **Local executable confirmation:** pending. The game installation is not available in this workspace, so 1.5.97 remains a strong target inference rather than a direct read of `SkyrimSE.exe`.
- **SKSE candidate:** 2.0.20 is the official SKSE build for runtime 1.5.97. The installed SKSE version has not been confirmed.

## Candidate inventory icon path

Dynamic Inventory Icon Injector (DIII) adds dynamic status icons to the inventory interface and has an API for SKSE plugin developers to register custom conditions. This fits the requirement for a separate personal-read status icon. Its currently published page does not clearly state support for Skyrim runtime 1.5.97, so runtime compatibility and Enderal behavior remain unverified. Do not make it a hard dependency or begin UI integration until this is confirmed.

Inventory Interface Information Injector (I4) injects item-type icons rather than status indicators. It may inform the UI architecture, but it does not by itself establish the required additional status icon path.

## Gates before plugin implementation

1. Verify the installed `SkyrimSE.exe` file version is 1.5.97 and identify the installed SKSE and SkyUI versions.
2. Confirm a compatible build of DIII can load on that runtime, or select a tested alternative that can add a conditional status icon beside SkyUI's read indicator.
3. Confirm a supported way to resolve the selected book in inventory and the open book form, register the configurable hotkey, and serialize marker data per save.
4. Record any UI reskin conflicts and test that the native read icon remains visible.

If a gate fails, update the design/plan with the supported route before implementing the affected component.

## Sources

- [Steam: Enderal SE update announcement](https://store.steampowered.com/app/976620/Enderal_Forgotten_Stories/) — maps Enderal 2.0.12.4 to Skyrim 1.5.97 and Enderal 2.1.4.4 to Skyrim 1.7.104.
- [SKSE official site](https://skse.silverlock.org/) — lists SKSE 2.0.20 for game version 1.5.97.
- [Dynamic Inventory Icon Injector (Nexus Mods)](https://www.nexusmods.com/skyrimspecialedition/mods/174136) — documents the dynamic status-icon feature and plugin API; runtime 1.5.97 compatibility still needs verification.
- [Inventory Interface Information Injector (Nexus Mods)](https://www.nexusmods.com/skyrimspecialedition/mods/85702) — documents item-icon injection; a separate 1.5.97 port exists, but I4 is not itself a status-icon solution.
