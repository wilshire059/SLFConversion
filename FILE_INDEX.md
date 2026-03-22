# SLFConversion Project - File Organization Index

All non-UE5 files have been organized into the following structure.
UE5 standard directories (Binaries, Config, Content, Intermediate, Plugins, Saved, Source) remain at root.

---

## Root (essential files only)
| File | Purpose |
|------|---------|
| `SLFConversion.uproject` | UE5 project file |
| `CompileBatch.bat` | Build script |
| `migration_tracker.md` | Active migration status (referenced by CLAUDE.md) |
| `BLUEPRINT_MIGRATION_CHECKLIST.md` | Blueprint checklist (referenced by migrate-all skill) |
| `.gitignore` / `.gitattributes` | Git config |

---

## `scripts/` — All Python, PowerShell, and batch scripts

| Folder | Count | Description |
|--------|-------|-------------|
| `scripts/migration/` | 371 | Apply, fix, configure, restore, patch, add, set, update, rename, delete, create, setup, migrate, move, copy, generate, build scripts |
| `scripts/extraction/` | 171 | Extract, export, dump, parse, get scripts — data extraction from Blueprints/assets |
| `scripts/validation/` | 457 | Check, verify, validate, compare, test scripts — validation and comparison tools |
| `scripts/debug/` | 29 | Debug and analyze scripts — diagnostic tools |
| `scripts/misc/` | 275 | Run scripts, PowerShell scripts, and other uncategorized scripts |
| `scripts/batch/` | 10 | Batch files for migration, testing, and automation |
| `scripts/blender/` | 6 | Blender pipeline scripts (build_proper_blend, extract_animations, mesh_animation_pipeline, etc.) |
| `scripts/tests/` | 44 | Python test files |

---

## `docs/` — Documentation and reports

| Folder | Count | Description |
|--------|-------|-------------|
| `docs/migration/` | 20 | Migration trackers, plans, checklists, postmortems, execution plans |
| `docs/migration/plans/` | 4 | YAML migration plans (B_Stat, B_StatusEffect, W_HUD, W_LevelUp) |
| `docs/migration/tracking/` | 2 dirs | Layer1_Interfaces, Layer3_Components tracking |
| `docs/reviews/` | 9 | Gemini peer reviews + GI/PC SoulslikeFramework reviews |
| `docs/animation/` | 6 | AnimBP fix reports, AnimGraph strategy, TAE format/reference, custom animation nodes |
| `docs/misc/` | 5 | Session logs, definition of done, healthcheck, validation strategy |
| `docs/npc/` | 8 dirs | NPC system documentation |

---

## `logs/` — Output and diagnostic logs (198 files)

All `.txt` output files from migration runs, build output, sentinel diagnostics, PIE tests,
import logs, diagnostic results, etc. Historical artifacts — kept for reference.

---

## `data/` — JSON and YAML data files

| File | Purpose |
|------|---------|
| `action_data.json` | Action system data |
| `affected_blueprints.json` | Blueprints affected by migration |
| `backup_widget_data.json` | Widget backup data |
| `blueprint_cdo_values.json` | CDO override values |
| `bulk_migration_results.json` | Batch migration results |
| `dungeon_dump.json` | Dungeon config dump |
| `extracted_item_data.json` | Item data extraction |
| `icon_paths.json` | Icon asset paths |
| `imagen_response.json` | Gemini image generation response |
| `item_info_backup_data.json` | Item info backups |
| `item_info_data.json` | Item info data |
| `niagara_paths.json` | Niagara system paths |
| `blueprints_inventory.yaml` | Blueprint inventory |
| `blueprints_inventory_comprehensive.yaml` | Comprehensive Blueprint inventory |
| `gap_analysis.yaml` | Migration gap analysis |
| `component_snapshots/` | Component state snapshots |

---

## Other existing directories (unchanged)

| Folder | Purpose |
|--------|---------|
| `Exports/` | Large JSON dumps from Blueprint extraction (gitignored) |
| `migration_cache/` | Cached icons, niagara, stats (survives restores) |
| `backups/` | Named backups (animbp_native_complete, etc.) |
| `tae_output/` | TAE parser output (untracked) |
