# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

SCION is a Skyrim Script Extender (SKSE) plugin that optimizes SkyUI container inventory interactions. It is built on **CommonLibSSE-NG** to support multiple Skyrim versions (SE, AE, GOG, VR). The project also includes the SkyUI Flash UI source code in `scionui/src/`, written in **ActionScript 2.0**, which implements Skyrim's SkyUI menus (container, inventory, barter, MCM, etc.).

## Build Commands

Requires Visual Studio 2022, CMake 3.21+, vcpkg with the custom registry configured, and a C++23-capable toolchain (MSVC 19.35+ / VS 2022 17.5+).

```bash
# Configure
cmake --preset build-debug-msvc    # Debug build
cmake --preset build-release-msvc  # Release build

# Build
cmake --build --preset debug-msvc
cmake --build --preset release-msvc
```

Build output goes to `build/debug-msvc/` or `build/release-msvc/`. There are no lint or test commands — the project relies on MSVC's built-in analysis and Clang-Tidy (configured in CMakePresets.json).

## Deployment

Set one of these environment variables to auto-deploy the compiled `.dll` post-build:

- `SKYRIM_MODS_FOLDER` — path to MO2/Vortex mods folder; deploys to a subfolder named `SCION - SkyUI Container Inventory Optimization NG`
- `SKYRIM_FOLDER` — path to Skyrim SE install root; deploys to `Data/`

The `config/` directory is deployed alongside the `.dll` to `SKSE/Plugins/Template_Plugin/`.

## Dependencies (via vcpkg)

- **commonlibsse-ng-fork** — Core Skyrim RE library. Source: custom registry at `https://github.com/Monitor221hz/modding-vcpkg-ports`
- **simpleini** — INI file parsing for runtime config
- **nlohmann-json** — JSON parsing
- **directxtk** — DirectX Toolkit (linked for UI rendering)
- **spdlog** — Logging (pulled in transitively via CommonLibSSE)

## Architecture

### Plugin Entry & Event Flow

`src/plugin.cpp` contains `SKSEPluginLoad()` — the only entry point SKSE calls. It initializes the log and registers a message handler for SKSE lifecycle events:

```
SKSEPluginLoad()
  → SKSE::Init()
  → SetupLog()        (src/log.h — writes to %SKSE_LOG_DIR%/SCION.log at trace level)
  → RegisterListener(MessageHandler)
      ├─ kDataLoaded
      ├─ kPostLoad
      ├─ kPreLoadGame
      ├─ kPostLoadGame
      └─ kNewGame
```

`kPreLoadGame` and `kNewGame` call `InventoryCacheManager::GetSingleton()->ClearAll()`. Other cases are stubs.

### Where to Implement Things

- **`src/MenuHooks.h` / `src/MenuHooks.cpp`** — VTable hooks for inventory menus; `InstallHooks()` called from `kDataLoaded`
- **`src/hook.h` / `src/hook.cpp`** — Reserved stubs; add non-menu hook groups as separate files (same pattern as MenuHooks)
- **`src/settings.h`** — Empty placeholder for INI/JSON config structures
- **`src/plugin.cpp`** — Wire up hooks and settings inside message handler cases

> **Adding new source files:** Every new `.h` must be added to `cmake/headerlist.cmake` and every new `.cpp` to `cmake/sourcelist.cmake`. CMake will not pick them up automatically.

### MenuHooks (`src/MenuHooks.h/.cpp`)

VTable hooks on `ProcessMessage` (slot 4) for `InventoryMenu`, `ContainerMenu`, and `BarterMenu`. On `kShow` or `kInventoryUpdate` (type 8): clears the cache, populates player inventory (+ target inventory for Container/Barter), calls the original, then invokes `InvalidateItemList` on the SWF root to signal the AS2 layer to discard the vanilla item array.

Install pattern: `REL::Relocation<std::uintptr_t>{ RE::VTABLE_*[0] }.write_vfunc(4, &thunk)` — returns the original as `uintptr_t`, cast to function pointer with `reinterpret_cast`.

### InventoryCacheManager (`src/InventoryCacheManager.h/.cpp`)

The primary inventory backend. Implements a **Master List + Active View** pattern:
- **Master List** — flat `std::vector<CachedItem>` of fully-resolved, UI-ready item snapshots
- **Active View** — `std::vector<CachedItem*>` of raw pointers into the master list; filtered, sorted, and ready to paginate
- **PointerMap** — `unordered_map<uint64_t, ItemPointers>` for O(1) lookup of live engine pointers by `SessionID`

Write path: `ClearAll()` → `StoreItem()` × N → `UpdateActiveView()` → `GetPage()`. Call sequence is enforced by the caller; the manager does not track dirty state.

Thread safety: write operations use `unique_lock<shared_mutex>`; reads use `shared_lock`.

### Cross-Version Compatibility

Use `RELOCATION_ID(SE_offset, AE_offset)` when calling any raw Skyrim engine function. This macro resolves the correct address at runtime for SE vs. AE. See examples in `src/util.h` (`TranslateTo`, `Play`, `Clone`).

### Utility Library (`src/util.h`)

A large (609-line) header-only utility file organized into namespaces. Key ones:

| Namespace | Purpose |
|---|---|
| `FormUtil::Parse` | Resolve `TESForm` from mod name + ID or `"ModName~FormID"` config strings |
| `NifUtil::Node` / `NifUtil::Armature` | Scene graph traversal, node cloning, bone attachment |
| `NifUtil::Collision` | Thread-safe Havok collision toggling |
| `MathUtil::Angle` | Quaternion/vector math, angle normalization |
| `KeyUtil` | Keyboard/gamepad input mapping and macro offsets |
| `SystemUtil::File` | Scan directories for `.ini` config files |

### SkyUI UI Layer (`scionui/`)

The `scionui/` directory contains the full SkyUI Flash/ActionScript UI source:

- **`scionui/src/`** — ActionScript 2.0 (AS2) source files (`.as`) and Flash project files (`.fla`)
- **`scionui/interface/`** — Compiled SWF output, deployed to Skyrim's `Data/Interface/` folder

**Language:** ActionScript 2.0 — not AS3. No strict typing enforcement; uses a `MovieClip`-based component model. SKSE calls go through `Common/skse.as`.

**Compilation:** Open `.fla` files in Adobe Flash Pro / Adobe Animate, then publish to produce `.swf` files. Place the resulting SWFs in `scionui/interface/`. There is no CMake integration for the AS2 layer.

**Module map:**

| Directory | Role |
|---|---|
| `CLIK/` | Flash component framework — buttons, scrollbars, sliders, base `UIComponent` |
| `Common/` | Shared utilities, SKSE bridge (`skse.as`), list/tab components |
| `ItemMenus/` | **Primary area of interest** — Container, Inventory, Barter, Gift menus + shared `ItemCard`, `CategoryList`, `BottomBar` |
| `ModConfigPanel/` | MCM dialogs and controls |
| `HUDWidgets/` | HUD overlay widgets |
| `CraftingMenu/` | Crafting/smithing menu |
| `FavoritesMenu/` | Favorites/quick-select menu |
| `MapMenu/` | World map UI |
| `MessageBox/` | Dialog/message box menus |
| `PauseMenu/` | Pause/system menu |
| `Resources/` | Shared assets (fonts, icons, etc.) |

**`version.as`** (at `scionui/src/version.as`) defines `SKYUI_VERSION_STRING`, `SKYUI_RELEASE_IDX`, and related version constants.

For container/inventory optimization work, start in `scionui/src/ItemMenus/`.

## CommonLibSSE-NG Gotchas

- **`GetRuntimeData()` is per-subclass, not on `IMenu`** — `InventoryMenu`, `ContainerMenu`, and `BarterMenu` each define their own `RUNTIME_DATA` struct (with `root` at different offsets). Cast `IMenu*` to the specific type before calling `GetRuntimeData()`.
- **VTABLE constants need `RE::` prefix** — `RE::VTABLE_InventoryMenu[0]`, `RE::VTABLE_ContainerMenu[0]`, `RE::VTABLE_BarterMenu[0]` (defined in `RE/Offsets_VTABLE.h` inside `namespace RE`).
- **`write_vfunc` returns `uintptr_t`** — cast to your function pointer type with `reinterpret_cast<Fn>(reloc.write_vfunc(idx, &thunk))`.
