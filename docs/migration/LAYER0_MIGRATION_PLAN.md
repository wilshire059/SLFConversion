# Layer 0 Migration Plan - Enums and Structs

## Status: LAYER 0 COMPLETE (December 28, 2025)

All C++ types created and compiling successfully.

**Files Created:**
- `Source/SLFConversion/SLFStatTypes.h` - All enum/struct definitions
- `Source/SLFConversion/SLFStatTypes.cpp` - Minimal implementation file

**Types Implemented:**
- ESLFValueType (enum)
- FSLFRegen (struct)
- FSLFStatBehavior (struct)
- FSLFStatInfo (struct)
- FSLFStatOverride (struct)

---

## Overview

This document details the migration of Blueprint-defined enums and structs to C++ equivalents.
This is a prerequisite for migrating TMap variables and functions from AC_StatManager.

**Goal:** Create C++ types that can replace Blueprint types, enabling:
- TMap variable migration (Stats, ActiveStats, StatOverrides)
- Function migration (GetStat, AdjustStat, GetAllStats, etc.)
- B_Stat reparenting to C++ base class

---

## Dependency Chain

```
LAYER 0 - No Dependencies (THIS DOCUMENT)
├── E_ValueType → ESLFValueType
├── FRegen → FSLFRegenInfo
├── FStatBehavior → FSLFStatBehavior
├── FStatInfo → FSLFStatInfo
└── FStatOverride → FSLFStatOverride

LAYER 3 - Depends on Layer 0
└── B_Stat → Reparent to USLFStatObject (uses FSLFStatInfo)

LAYER 4 - Depends on Layer 3
├── Stats TMap → TMap<TSubclassOf<USLFStatObject>, FGameplayTag>
├── ActiveStats TMap → TMap<FGameplayTag, USLFStatObject*>
├── StatOverrides TMap → TMap<FGameplayTag, FSLFStatOverride>
└── Functions → C++ implementations
```

---

## Type Definitions from Blueprint JSON Exports

### E_ValueType (Enum)
**Source:** `/Game/SoulslikeFramework/Enums/E_ValueType`

| Internal Name | Display Name | Value |
|---------------|--------------|-------|
| NewEnumerator0 | Current Value | 0 |
| NewEnumerator1 | Max Value | 1 |

### FRegen (Struct)
**Source:** `/Game/SoulslikeFramework/Structures/Stats/FRegen`

| Property | Type | Default |
|----------|------|---------|
| CanRegenerate? | bool | false |
| RegenPercent | float | 0.0 |
| RegenInterval | float | 0.0 |

### FStatBehavior (Struct)
**Source:** `/Game/SoulslikeFramework/Structures/Stats/FStatBehavior`

| Property | Type | Default |
|----------|------|---------|
| StatsToAffect | TMap<FGameplayTag, double> | () |

*Note: TMap key/value types inferred from usage in AdjustAffected functions*

### FStatInfo (Struct)
**Source:** `/Game/SoulslikeFramework/Structures/Stats/FStatInfo`

| Property | Type | Default |
|----------|------|---------|
| Tag | FGameplayTag | "" |
| DisplayName | FText | "" |
| Description | FText | "" |
| DisplayAsPercent? | bool | false |
| CurrentValue | double | 0.0 |
| MaxValue | double | 0.0 |
| ShowMaxValue? | bool | false |
| RegenInfo | FFRegen | (default) |
| StatModifiers | FFStatBehavior | () |

### FStatOverride (Struct)
**Source:** `/Game/SoulslikeFramework/Structures/Stats/FStatOverride`

| Property | Type | Default |
|----------|------|---------|
| OverrideCurrentValue | double | 0.0 |
| OverrideMaxValue | double | 0.0 |
| OverrideRegen? | bool | false |
| OverrideRegenInfo | FFRegen | (default) |

---

## C++ Implementation Plan

### File: `SLFStatTypes.h` (NEW)

All Layer 0 types in a single header for simplicity.

```cpp
// Order matters - dependencies first
1. ESLFValueType (enum) - no dependencies
2. FSLFRegenInfo (struct) - no dependencies
3. FSLFStatBehavior (struct) - depends on FGameplayTag
4. FSLFStatInfo (struct) - depends on FSLFRegenInfo, FSLFStatBehavior
5. FSLFStatOverride (struct) - depends on FSLFRegenInfo
```

### Property Naming Convention

Blueprint uses names like `CanRegenerate?_2_930C52D54A3CFC505563F58B34E72C0D`.
C++ will use clean names: `bCanRegenerate`.

The migration script will handle the mapping.

---

## Migration Strategy

### Phase 1: Create C++ Types (Current)
1. Create `SLFStatTypes.h` with all enum/struct definitions
2. Compile and verify no errors
3. Do NOT modify any Blueprints yet

### Phase 2: Blueprint Type Migration (Future)
For each Blueprint using these types:
1. Create migration script to replace BP type references with C++ types
2. Update pin connections
3. Delete old BP types

### Phase 3: B_Stat Reparenting (Future)
1. Update USLFStatObject to use FSLFStatInfo
2. Reparent B_Stat to USLFStatObject
3. Migrate B_Stat variables to inherited C++ properties

### Phase 4: TMap and Function Migration (Future)
1. Update SoulslikeStatComponent TMap types
2. Migrate AC_StatManager functions to C++
3. Update all Blueprint references

---

## Validation Checklist

- [x] ESLFValueType enum compiles
- [x] FSLFRegen struct compiles
- [x] FSLFStatBehavior struct compiles
- [x] FSLFStatInfo struct compiles
- [x] FSLFStatOverride struct compiles
- [x] All types are BlueprintType
- [x] All properties are BlueprintReadWrite
- [x] Property types match Blueprint exactly (float vs double, etc.)

---

## Notes

- Blueprint uses `FFRegen` internally (double F prefix) - this is just Blueprint's naming convention
- FStatBehavior.StatsToAffect TMap type needs verification during implementation
- All structs should match Blueprint struct sizes for binary compatibility

---

## Next Steps: Layer 3 (B_Stat Reparenting)

Now that Layer 0 types exist, the next step is to:

1. **Update USLFStatObject** to include FSLFStatInfo as a property
2. **Reparent B_Stat** to USLFStatObject
3. **Migrate B_Stat variables** to inherited C++ properties
4. **Update SoulslikeStatComponent TMaps** to use USLFStatObject instead of UObject

This will enable:
- Type-safe TMap variables (Stats, ActiveStats)
- Function migration to C++

See LAYER3_MIGRATION_PLAN.md (to be created) for details.
