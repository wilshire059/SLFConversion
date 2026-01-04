# Data Flow Analysis Guide

## Overview

This guide explains how to trace data flow through Blueprint graphs to identify:
- Runtime setup requirements (visual components configured in EventGraph)
- Data asset dependencies and their struct types
- Reflection requirements for Blueprint-defined structs

**Perform this analysis BEFORE writing any C++ implementation code.**

---

## Visual Component Detection

### Patterns to Search For

Scan the EventGraph JSON for these function call patterns:

| Node Pattern | What It Means | C++ Requirement |
|--------------|---------------|-----------------|
| `Set Asset` on Niagara | Niagara configured at runtime | Reproduce in BeginPlay |
| `SetStaticMesh` | Mesh assigned at runtime | Reproduce in BeginPlay |
| `SetSkeletalMesh` | Skeletal mesh assigned at runtime | Reproduce in BeginPlay |
| `SetMaterial` / `SetMaterialByName` | Material set dynamically | Reproduce in BeginPlay |
| `Set Brush from Texture` | UI texture set at runtime | Reproduce in Construct |

### JSON Search Patterns

```json
// Look for K2Node_CallFunction with these FunctionReference names:
"FunctionReference": {
  "MemberName": "SetAsset"  // Niagara
}

"FunctionReference": {
  "MemberName": "SetStaticMesh"
}

"FunctionReference": {
  "MemberName": "SetSkeletalMesh"
}
```

### What This Means

If visual components are configured at **runtime** (via EventGraph):
1. The component itself should be preserved (use `KEEP_VARS_MAP`)
2. The C++ class must reproduce the setup logic in `BeginPlay()`
3. Trace the data SOURCE to understand where the asset reference comes from

---

## Data Source Tracing Workflow

### Step 1: Find "Get" Nodes

In the EventGraph JSON, look for:
- `K2Node_VariableGet` - Reading a variable
- `K2Node_CallFunction` with getter patterns (`Get*`, `TryGet*`)
- `BreakStruct` nodes that access struct properties

### Step 2: Identify the Variable

For each "Get" node, identify:
- What variable is being read?
- What is its type? (UDataAsset*, UObject*, struct, etc.)
- Is it a member variable or passed as parameter?

### Step 3: Check Data Asset Type

If the variable is a `UDataAsset*`:

| Data Asset Class Pattern | Is Blueprint? | Access Method |
|--------------------------|---------------|---------------|
| `UPDA_*` (C++ class) | No | Direct cast |
| `PDA_*_C` (Blueprint suffix) | Yes | Reflection |
| Path contains `/Game/` | Yes | Reflection |
| Path contains `/Script/` | No | Direct cast |

### Step 4: Trace Struct Properties

If the data asset contains struct properties:
1. Is the struct a C++ USTRUCT (`FSLFItemInfo`)?
2. Or a Blueprint struct (`FItemInfo`, path has `/Game/`)?
3. Blueprint structs have property names with GUID suffixes

---

## Reflection Requirement Decision Tree

```
Is the variable a UDataAsset*?
├─ NO → Regular C++ access
└─ YES → What class is it?
    ├─ C++ class (UPDA_*) → Does it contain FInstancedStruct?
    │   ├─ NO → Direct property access
    │   └─ YES → What struct type is stored?
    │       ├─ C++ USTRUCT → GetPtr<T>() works
    │       └─ Blueprint struct → TFieldIterator + prefix matching
    └─ Blueprint class (PDA_*) → REFLECTION REQUIRED
        └─ FindPropertyByName + ContainerPtrToValuePtr
            └─ For nested structs → TFieldIterator + prefix matching
```

---

## Nested Struct Traversal Pattern

When accessing Blueprint struct properties from C++:

### The Problem

Blueprint struct property names include GUID suffixes:
```
Expected: "WorldPickupInfo"
Actual:   "WorldPickupInfo_50_ABC123..."
```

Using `FindPropertyByName("WorldPickupInfo")` will FAIL.

### The Solution: Prefix Matching

```cpp
// Template for traversing Blueprint struct hierarchy
bool TraverseNestedBlueprintStruct(
    UObject* DataAsset,
    const TCHAR* OuterPropName,
    const TCHAR* NestedPropPrefix,
    void** OutNestedPtr)
{
    // 1. Get outer property by exact name (top-level props don't have GUIDs)
    FProperty* OuterProp = DataAsset->GetClass()->FindPropertyByName(OuterPropName);
    if (!OuterProp) return false;

    const void* OuterPtr = OuterProp->ContainerPtrToValuePtr<void>(DataAsset);

    // 2. Get struct type for iteration
    FStructProperty* StructProp = CastField<FStructProperty>(OuterProp);
    if (!StructProp) return false;

    UScriptStruct* StructType = StructProp->Struct;

    // 3. Find nested property by PREFIX (handles GUID suffix)
    for (TFieldIterator<FProperty> PropIt(StructType); PropIt; ++PropIt)
    {
        if (PropIt->GetName().StartsWith(NestedPropPrefix))
        {
            *OutNestedPtr = (*PropIt)->ContainerPtrToValuePtr<void>(
                const_cast<void*>(OuterPtr));
            return true;
        }
    }

    return false;
}
```

### Required Includes

```cpp
#include "UObject/UnrealType.h"  // TFieldIterator, FProperty, FStructProperty
```

---

## Property Type Extraction

### Soft Object References

```cpp
// For TSoftObjectPtr<T> in Blueprint struct
FSoftObjectProperty* SoftObjProp = CastField<FSoftObjectProperty>(Prop);
if (SoftObjProp)
{
    const FSoftObjectPtr* SoftPtr = Prop->ContainerPtrToValuePtr<FSoftObjectPtr>(StructData);
    if (SoftPtr && !SoftPtr->IsNull())
    {
        UObject* LoadedObj = SoftPtr->LoadSynchronous();
        // Cast to expected type
    }
}
```

### Object References

```cpp
// For UObject* in Blueprint struct
FObjectProperty* ObjProp = CastField<FObjectProperty>(Prop);
if (ObjProp)
{
    void* PropAddr = Prop->ContainerPtrToValuePtr<void>(const_cast<void*>(StructData));
    UObject* Obj = ObjProp->GetObjectPropertyValue(PropAddr);
    // Cast to expected type
}
```

### Primitive Types

```cpp
// For bool, int32, float, etc.
void* PropAddr = Prop->ContainerPtrToValuePtr<void>(const_cast<void*>(StructData));

// Bool
if (FBoolProperty* BoolProp = CastField<FBoolProperty>(Prop))
{
    bool Value = BoolProp->GetPropertyValue(PropAddr);
}

// Int32
if (FIntProperty* IntProp = CastField<FIntProperty>(Prop))
{
    int32 Value = *static_cast<int32*>(PropAddr);
}

// Float
if (FFloatProperty* FloatProp = CastField<FFloatProperty>(Prop))
{
    float Value = *static_cast<float*>(PropAddr);
}
```

---

## Complete Example: B_PickupItem

### Analysis Results

**Variable:** `Item` (UDataAsset*)

**Data Asset Class:** `PDA_Item_C` (Blueprint-derived)

**Struct Hierarchy:**
```
Item (PDA_Item)
  └─ ItemInformation (FItemInfo - Blueprint struct)
       └─ WorldPickupInfo (FWorldMeshInfo - Blueprint struct)
            └─ WorldNiagaraSystem (TSoftObjectPtr<UNiagaraSystem>)
```

**Runtime Setup Found:**
- EventGraph calls "Set Niagara System Asset"
- Source: `Item.ItemInformation.WorldPickupInfo.WorldNiagaraSystem`

### C++ Implementation

```cpp
void ASLFPickupItemBase::SetupWorldNiagara()
{
    UNiagaraComponent* NiagaraComp = FindComponentByClass<UNiagaraComponent>();
    if (!NiagaraComp || !Item) return;

    // 1. Get ItemInformation from Blueprint data asset
    UClass* ItemClass = Item->GetClass();
    FProperty* ItemInfoProp = ItemClass->FindPropertyByName(TEXT("ItemInformation"));
    if (!ItemInfoProp) return;

    const void* ItemInfoPtr = ItemInfoProp->ContainerPtrToValuePtr<void>(Item);
    FStructProperty* ItemInfoStructProp = CastField<FStructProperty>(ItemInfoProp);
    UScriptStruct* ItemInfoStruct = ItemInfoStructProp->Struct;

    // 2. Find WorldPickupInfo by prefix
    FProperty* WorldPickupInfoProp = nullptr;
    for (TFieldIterator<FProperty> PropIt(ItemInfoStruct); PropIt; ++PropIt)
    {
        if (PropIt->GetName().StartsWith(TEXT("WorldPickupInfo")))
        {
            WorldPickupInfoProp = *PropIt;
            break;
        }
    }
    if (!WorldPickupInfoProp) return;

    const void* WorldPickupInfoPtr = WorldPickupInfoProp->ContainerPtrToValuePtr<void>(ItemInfoPtr);
    FStructProperty* WorldPickupStructProp = CastField<FStructProperty>(WorldPickupInfoProp);
    UScriptStruct* WorldPickupStruct = WorldPickupStructProp->Struct;

    // 3. Find WorldNiagaraSystem by prefix
    FProperty* NiagaraSystemProp = nullptr;
    for (TFieldIterator<FProperty> PropIt(WorldPickupStruct); PropIt; ++PropIt)
    {
        if (PropIt->GetName().StartsWith(TEXT("WorldNiagaraSystem")))
        {
            NiagaraSystemProp = *PropIt;
            break;
        }
    }
    if (!NiagaraSystemProp) return;

    // 4. Load soft reference
    const FSoftObjectPtr* SoftObjectPtr = NiagaraSystemProp->ContainerPtrToValuePtr<FSoftObjectPtr>(WorldPickupInfoPtr);
    if (!SoftObjectPtr || SoftObjectPtr->IsNull()) return;

    UNiagaraSystem* NiagaraSystem = Cast<UNiagaraSystem>(SoftObjectPtr->LoadSynchronous());
    if (NiagaraSystem)
    {
        NiagaraComp->SetAsset(NiagaraSystem);
        NiagaraComp->Activate(true);
    }
}
```

---

## Pre-Migration Checklist

Before implementing any Blueprint in C++:

### Component Preservation
- [ ] List all SCS components from Blueprint JSON
- [ ] For each visual component (Mesh, Niagara, etc.):
  - [ ] Check if asset is set in defaults
  - [ ] Check if EventGraph sets the asset
  - [ ] If runtime setup → use KEEP_VARS_MAP + reproduce in C++

### Data Asset Dependencies
- [ ] List all UDataAsset* variables
- [ ] For each data asset:
  - [ ] Check class (C++ UPDA_* or Blueprint PDA_*?)
  - [ ] If Blueprint → flag for reflection
  - [ ] Map struct property hierarchy

### Reflection Requirements
- [ ] For each Blueprint struct:
  - [ ] Document the property path
  - [ ] Note which properties need prefix matching
  - [ ] Identify nested struct traversal order

### Document Before Coding
- [ ] Write down the complete data flow path
- [ ] Write down the reflection traversal order
- [ ] Identify all soft references that need LoadSynchronous()
