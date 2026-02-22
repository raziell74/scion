# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

SCION is a Skyrim Script Extender (SKSE) plugin that optimizes SkyUI container inventory interactions. It is built on **CommonLibSSE-NG** to support multiple Skyrim versions (SE, AE, GOG, VR). The project also includes the SkyUI Flash UI source code in `scionui/src/`, written in **ActionScript 2.0**, which implements Skyrim's SkyUI menus (container, inventory, barter, MCM, etc.).

## Build Commands

Requires Visual Studio 2022, CMake 3.21+, and vcpkg with the custom registry configured.

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

All message handler cases are currently empty stubs — implementation goes here.

### Where to Implement Things

- **`src/hook.h` / `src/hook.cpp`** — Empty placeholders for Skyrim function hooks
- **`src/settings.h`** — Empty placeholder for INI/JSON config structures
- **`src/plugin.cpp`** — Wire up hooks and settings inside the message handler cases

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
