# SoulslikeFramework Migration Execution Plan

## Document Purpose

This document defines the **exact execution plan** for migrating the SoulslikeFramework from Blueprint to C++ using the Shell Pattern. It includes the validation system, tracking mechanisms, and step-by-step procedures.

---

## Table of Contents

1. [Overview](#overview)
2. [The Shell Pattern](#the-shell-pattern)
3. [Migration Inventory](#migration-inventory)
4. [Validation System (3-Pass)](#validation-system-3-pass)
5. [Migration Tracking Schema](#migration-tracking-schema)
6. [Layer-by-Layer Execution](#layer-by-layer-execution)
7. [Post-Migration Debugging](#post-migration-debugging)
8. [File Locations](#file-locations)

---

## Overview

### Goal
Convert ALL Blueprint logic to C++ while preserving Blueprint "shells" that inherit from C++ base classes. This preserves all asset references while moving performance-critical code to C++.

### Approach
1. Restore clean baseline from `C:\scripts\bp_only`
2. Write ALL C++ base classes
3. Create Python migration scripts
4. Execute migration in single pass
5. Debug iteratively until stable

### Key Principle
**No shortcuts.** Every variable, function, and graph node must be:
- Extracted to tracking JSON
- Reviewed in 3 passes
- Validated against original Blueprint
- Documented with implementation summary

---

## The Shell Pattern

### What It Is
```
BEFORE:                           AFTER:
┌─────────────────────┐          ┌─────────────────────┐
│ Blueprint           │          │ C++ Base Class      │
│ - Variables         │          │ - All Variables     │
│ - Functions         │          │ - All Functions     │
│ - Event Graphs      │          │ - All Logic         │
│ - Component Refs    │          └──────────┬──────────┘
└─────────────────────┘                     │ inherits
                                 ┌──────────▼──────────┐
                                 │ Blueprint Shell     │
                                 │ - Data Asset Refs   │
                                 │ - Mesh Refs         │
                                 │ - Settings Only     │
                                 └─────────────────────┘
```

### Benefits
- All asset paths preserved (no broken references)
- Blueprint children automatically inherit C++ logic
- Visual settings remain editable in Blueprint
- Full C++ performance for logic

---

## Migration Inventory

### Summary Table

| Layer | Category | Count | Status |
|-------|----------|-------|--------|
| 0 | Enums | 39 | DONE (SLFEnums.h) |
| 0 | Structs | 95 | MOSTLY DONE (SLFGameTypes.h) |
| 1 | Interfaces | 18 | NOT STARTED |
| 2 | Data Asset Bases | ~15 | NOT STARTED |
| 3 | Components | 21 | 4 DONE, 17 REMAINING |
| 4 | Core Blueprints | ~8 | NOT STARTED |
| 5 | Character Blueprints | ~10 | NOT STARTED |
| 6 | Game Framework | 5 | NOT STARTED |
| 7 | Animation Blueprints | 8 | KEEP AS BP |
| 8 | Widgets | 122 | KEEP AS BP |

### Detailed Inventory by Layer

#### Layer 0: Enums (39) - COMPLETE
All enums defined in `SLFEnums.h`

#### Layer 0: Structs (95) - MOSTLY COMPLETE
Most structs defined in `SLFGameTypes.h` and `SLFStatTypes.h`

#### Layer 1: Interfaces (18) - TO DO
```
BPI_GenericCharacter    BPI_Player           BPI_Enemy
BPI_NPC                 BPI_Controller       BPI_AIC
BPI_Item                BPI_Interactable     BPI_Projectile
BPI_Executable          BPI_ExecutionIndicator   BPI_DestructibleHelper
BPI_BossDoor            BPI_RestingPoint     BPI_EnemyHealthbar
BPI_GameInstance        BPI_MainMenu         BPI_StatEntry
```

#### Layer 3: Components (21)
**Done (4):**
- UInputBufferComponent
- UActionManagerComponent
- UBuffManagerComponent
- UStatManagerComponent

**To Do (17):**
```
AC_CombatManager         AC_InteractionManager    AC_EquipmentManager
AC_InventoryManager      AC_StatusEffectManager   AC_CollisionManager
AC_LadderManager         AC_LootDropManager       AC_ProgressManager
AC_SaveLoadManager       AC_RadarManager          AC_RadarElement
AC_DebugCentral          AC_AI_BehaviorManager    AC_AI_Boss
AC_AI_CombatManager      AC_AI_InteractionManager
```

#### Layer 4: Core Blueprints (~8)
```
B_Stat           B_Action         B_Buff
B_StatusEffect   B_Item           B_Interactable
B_BaseProjectile B_AbilityEffectBase
```

#### Layer 5: Character Blueprints (~10)
```
B_BaseCharacter          B_Soulslike_Character    B_Soulslike_Enemy
B_Soulslike_Enemy_Guard  B_Soulslike_Boss         B_Soulslike_Boss_Malgareth
B_Soulslike_NPC          B_Soulslike_NPC_ShowcaseGuide
B_Soulslike_NPC_ShowcaseVendor
```

#### Layer 6: Game Framework (5)
```
GI_SoulslikeFramework    GM_SoulslikeFramework    GM_Menu_SoulslikeFramework
PC_SoulslikeFramework    PC_Menu_SoulslikeFramework
```

---

## Validation System (3-Pass)

### Overview

Every migration requires **3 passes** with documented validation:

| Pass | Purpose | Checklist Focus |
|------|---------|-----------------|
| Pass 1 | Initial Implementation | Extract all items, write C++ |
| Pass 2 | Logic Validation | Verify each function matches JSON graph |
| Pass 3 | Dependency Validation | Verify all references resolve correctly |

### Pass 1: Initial Implementation

**For each Blueprint:**
1. Extract to tracking JSON (see schema below)
2. Write C++ header with all:
   - Variables (UPROPERTY)
   - Functions (UFUNCTION)
   - Event dispatchers (DECLARE_DYNAMIC_MULTICAST_DELEGATE)
3. Write C++ implementation
4. Mark items as "pass1_complete" in tracking JSON

**Checklist per item:**
- [ ] Variable name matches exactly (no 'b' prefix unless Blueprint has it)
- [ ] Variable type matches exactly
- [ ] Variable category matches
- [ ] Default value preserved
- [ ] BlueprintReadWrite/BlueprintReadOnly correct

### Pass 2: Logic Validation

**For each function:**
1. Read original Blueprint graph from JSON export
2. Trace exact control flow
3. Verify C++ implementation matches:
   - Branch conditions
   - True/False paths
   - Return values
   - Side effects
4. Mark as "pass2_validated" in tracking JSON
5. Write summary of what the function does

**Critical checks:**
- [ ] Branch conditions not inverted
- [ ] All execution paths covered
- [ ] Timer callbacks match original
- [ ] Event bindings preserved
- [ ] Async operations handled correctly

### Pass 3: Dependency Validation

**For each item:**
1. Verify all type references exist in C++
2. Verify all function calls resolve
3. Verify no circular dependencies
4. Mark as "pass3_complete" in tracking JSON

**Checklist:**
- [ ] All struct types exist
- [ ] All enum types exist
- [ ] All interface types exist
- [ ] All component references valid
- [ ] No forward declaration issues

---

## Migration Tracking Schema

### JSON Schema per Blueprint

Location: `C:/scripts/SLFConversion/MigrationTracking/<LayerN>/<BlueprintName>.json`

```json
{
  "blueprint_name": "AC_ActionManager",
  "blueprint_path": "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager",
  "json_export_path": "C:/scripts/SLFConversion_Migration/Backups/SoulsClean/Exports/BlueprintDNA/Component/AC_ActionManager.json",
  "cpp_header": "ActionManagerComponent.h",
  "cpp_source": "ActionManagerComponent.cpp",
  "parent_class": "UActorComponent",
  "implemented_interfaces": [],

  "migration_status": {
    "pass1_complete": false,
    "pass2_validated": false,
    "pass3_complete": false,
    "overall_status": "not_started"
  },

  "variables": [
    {
      "name": "IsSprinting",
      "type": "bool",
      "category": "Runtime",
      "default_value": "false",
      "blueprint_read_write": true,
      "pass1_complete": false,
      "pass2_validated": false,
      "pass3_complete": false,
      "notes": ""
    }
  ],

  "functions": [
    {
      "name": "PerformAction",
      "return_type": "void",
      "parameters": [
        {"name": "ActionTag", "type": "FGameplayTag"}
      ],
      "is_blueprint_native_event": true,
      "is_blueprint_pure": false,
      "is_blueprint_callable": true,
      "graph_summary": "",
      "pass1_complete": false,
      "pass2_validated": false,
      "pass3_complete": false,
      "implementation_summary": "",
      "validation_notes": ""
    }
  ],

  "event_dispatchers": [
    {
      "name": "OnInputBufferConsumed",
      "delegate_signature": "FGameplayTag",
      "pass1_complete": false,
      "pass2_validated": false,
      "pass3_complete": false
    }
  ],

  "graphs": [
    {
      "name": "EventGraph",
      "type": "event_graph",
      "entry_points": ["BeginPlay", "EndPlay"],
      "pass1_complete": false,
      "pass2_validated": false,
      "pass3_complete": false,
      "implementation_summary": ""
    }
  ],

  "collapsed_graphs": [],

  "dependencies": {
    "structs": ["FSLFDodgeMontages"],
    "enums": ["ESLFDirection"],
    "interfaces": [],
    "components": ["UStatManagerComponent"],
    "other_blueprints": []
  },

  "final_summary": "",
  "known_issues": [],
  "test_results": []
}
```

---

## Layer-by-Layer Execution

### Phase 1: Restore Baseline

```powershell
# 1. Close Unreal Editor
# 2. Remove current Content folder
Remove-Item -Recurse -Force "C:\scripts\SLFConversion\Content"

# 3. Copy clean baseline
Copy-Item -Recurse "C:\scripts\bp_only\Content" "C:\scripts\SLFConversion\"
```

### Phase 2: Write All C++ (in order)

**Order matters - dependencies must exist before dependents:**

```
Layer 1: Interfaces (18)
    └─ All BPI_* classes

Layer 2: Data Asset Bases (~15)
    └─ PDA_Action, PDA_Buff, PDA_StatusEffect, etc.

Layer 3: Components (17 remaining)
    └─ Priority: CombatManager, StatusEffectManager, InventoryManager
    └─ Then: Equipment, Interaction, AI components
    └─ Finally: Utility components

Layer 4: Core Blueprints (~8)
    └─ B_Stat, B_Action, B_Buff, B_StatusEffect
    └─ B_Item, B_Interactable, B_BaseProjectile

Layer 5: Character Blueprints (~10)
    └─ B_BaseCharacter first
    └─ Then: B_Soulslike_Character, B_Soulslike_Enemy
    └─ Then: Children (Boss, NPC, etc.)

Layer 6: Game Framework (5)
    └─ GameInstance, GameMode, PlayerController
```

### Phase 3: Create Migration Scripts

```
C:/scripts/SLFConversion/
├── migrate_all.py                    # Master script
├── migrate_layer1_interfaces.py
├── migrate_layer2_dataassets.py
├── migrate_layer3_components.py
├── migrate_layer4_blueprints.py
├── migrate_layer5_characters.py
└── migrate_layer6_framework.py
```

### Phase 4: Execute Migration

```python
# In Unreal Editor Python console:
exec(open(r"C:\scripts\SLFConversion\migrate_all.py").read())
```

### Phase 5: Debug Loop

```
LOOP:
  1. User: Compile Blueprints, test in editor
  2. User: Report errors/broken functionality
  3. Claude: Analyze and fix C++ implementation
  4. GOTO 1 until stable
```

---

## Post-Migration Debugging

### Error Categories

| Error Type | Likely Cause | Resolution |
|------------|--------------|------------|
| "Variable not found" | Name mismatch | Check exact BP name in JSON |
| "Function signature mismatch" | Parameter type/name wrong | Match JSON export exactly |
| "Pure function has exec pin" | Wrong UFUNCTION specifier | Use BlueprintPure |
| "Event not found" | "Event " prefix issue | Function exists, fix caller |
| "Cyclic dependency" | Include order wrong | Use forward declarations |

### Debugging Workflow

1. **Compile Error:**
   - Check Output Log for specific line/file
   - Cross-reference with tracking JSON
   - Verify against Blueprint JSON export

2. **Runtime Error:**
   - Add UE_LOG statements
   - Check function entry/exit
   - Verify variable values

3. **Functional Bug:**
   - Compare behavior to original Blueprint
   - Trace through JSON graph nodes
   - Check branch conditions

---

## File Locations

### Source Files

| Type | Location |
|------|----------|
| C++ Source | `C:/scripts/SLFConversion/Source/SLFConversion/` |
| Migration Scripts | `C:/scripts/SLFConversion/migrate_*.py` |
| Tracking JSONs | `C:/scripts/SLFConversion/MigrationTracking/` |
| Blueprint JSON Exports | `C:/scripts/SLFConversion_Migration/Backups/SoulsClean/Exports/BlueprintDNA/` |

### Backups

| Backup | Purpose |
|--------|---------|
| `C:/scripts/bp_only/` | Clean baseline (restore point) |
| `C:/scripts/backups/` | Point-in-time backups |

### Skills

| Skill | Location |
|-------|----------|
| Migration Skill | `C:/scripts/SLFConversion/.claude/skills/slf-migration/SKILL.md` |

---

## Checklist Summary

### Before Starting Migration
- [ ] Content restored from bp_only
- [ ] C++ compiles successfully
- [ ] All tracking JSONs generated
- [ ] Skill installed and recognized

### Per-Item Migration
- [ ] Pass 1: Implementation complete
- [ ] Pass 2: Logic validated against JSON
- [ ] Pass 3: Dependencies verified
- [ ] Implementation summary written
- [ ] Tracking JSON updated

### Post-Migration
- [ ] All Blueprints compile
- [ ] PIE mode works
- [ ] Core systems functional:
  - [ ] Combat (attack, dodge, block)
  - [ ] Stats (health, stamina, FP)
  - [ ] Buffs (apply, remove)
  - [ ] Inventory (add, remove, equip)
  - [ ] Save/Load

---

*Document Version: 1.0*
*Created: December 31, 2025*
