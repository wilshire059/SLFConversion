# Blueprint Struct Extraction from FInstancedStruct

## Problem Statement

When a Blueprint data asset stores data in an `FInstancedStruct`, the data is often a **Blueprint-defined struct** (not a C++ USTRUCT). Attempting to extract it with the C++ struct type fails:

```cpp
// FAILS - Blueprint struct "FDodgeMontages" != C++ struct "FSLFDodgeMontages"
if (const FSLFDodgeMontages* Data = InstancedStruct->GetPtr<FSLFDodgeMontages>())
{
    // Never reaches here
}
```

**Why it fails:** `GetPtr<T>()` checks if `T::StaticStruct() == InstancedStruct->GetScriptStruct()`. Blueprint structs are different `UScriptStruct*` instances even if they have identical fields.

---

## Solution: Prefix-Based Reflection

Use `TFieldIterator` to iterate properties and `PropName.StartsWith(Prefix)` to match, since Blueprint struct property names have GUID suffixes.

### Blueprint Struct Property Names

Blueprint-defined structs have property names with GUID suffixes:

| Expected Name | Actual Blueprint Name |
|---------------|----------------------|
| `Forward` | `Forward_31_D52898724986107771E21081C6388DEC` |
| `Backstep` | `Backstep_39_445171EA492D2774EC7E05B1573B0D28` |
| `Left` | `Left_40_...` |
| `Right` | `Right_41_...` |

---

## Complete Pattern

```cpp
#include "StructUtils/InstancedStruct.h"
#include "UObject/UnrealType.h"  // Required for TFieldIterator

void ExtractFromInstancedStruct(UDataAsset* Action, FSLFDodgeMontages& OutMontages)
{
    // Step 1: Try direct C++ cast first
    if (UPDA_Action* ActionData = Cast<UPDA_Action>(Action))
    {
        if (const FSLFDodgeMontages* Data = ActionData->RelevantData.GetPtr<FSLFDodgeMontages>())
        {
            OutMontages = *Data;
            return;
        }
    }

    // Step 2: Blueprint data asset - use reflection
    FProperty* RelevantDataProp = Action->GetClass()->FindPropertyByName(FName("RelevantData"));
    if (!RelevantDataProp) return;

    FStructProperty* StructProp = CastField<FStructProperty>(RelevantDataProp);
    if (!StructProp || StructProp->Struct->GetFName() != FName("InstancedStruct")) return;

    void* PropAddr = RelevantDataProp->ContainerPtrToValuePtr<void>(Action);
    FInstancedStruct* InstancedStruct = static_cast<FInstancedStruct*>(PropAddr);
    if (!InstancedStruct || !InstancedStruct->IsValid()) return;

    const UScriptStruct* StoredStructType = InstancedStruct->GetScriptStruct();
    const void* StructData = InstancedStruct->GetMemory();

    // Step 3: Extract properties by prefix
    auto GetMontageFromPropPrefix = [&](const TCHAR* Prefix) -> UAnimMontage* {
        for (TFieldIterator<FProperty> PropIt(StoredStructType); PropIt; ++PropIt)
        {
            FProperty* Prop = *PropIt;
            FString PropName = Prop->GetName();
            if (PropName.StartsWith(Prefix))
            {
                // Handle UAnimMontage* (direct reference)
                if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Prop))
                {
                    void* PropValueAddr = Prop->ContainerPtrToValuePtr<void>(const_cast<void*>(StructData));
                    UObject* Obj = ObjProp->GetObjectPropertyValue(PropValueAddr);
                    return Cast<UAnimMontage>(Obj);
                }
                // Handle TSoftObjectPtr<UAnimMontage> (soft reference)
                else if (FSoftObjectProperty* SoftObjProp = CastField<FSoftObjectProperty>(Prop))
                {
                    void* PropValueAddr = Prop->ContainerPtrToValuePtr<void>(const_cast<void*>(StructData));
                    TSoftObjectPtr<UObject>* SoftPtr = static_cast<TSoftObjectPtr<UObject>*>(PropValueAddr);
                    if (SoftPtr)
                    {
                        return Cast<UAnimMontage>(SoftPtr->LoadSynchronous());
                    }
                }
            }
        }
        return nullptr;
    };

    OutMontages.Fwd = GetMontageFromPropPrefix(TEXT("Forward"));
    OutMontages.Bwd = GetMontageFromPropPrefix(TEXT("Backstep"));
    OutMontages.Left = GetMontageFromPropPrefix(TEXT("Left"));
    OutMontages.Right = GetMontageFromPropPrefix(TEXT("Right"));
}
```

---

## Required Includes

```cpp
#include "StructUtils/InstancedStruct.h"   // FInstancedStruct
#include "UObject/UnrealType.h"            // TFieldIterator, FProperty
```

---

## Property Type Handlers

| Blueprint Type | FProperty Subclass | Extraction Method |
|----------------|-------------------|-------------------|
| Object Reference | `FObjectProperty` | `ObjProp->GetObjectPropertyValue(addr)` |
| Soft Object Reference | `FSoftObjectProperty` | `TSoftObjectPtr->LoadSynchronous()` |
| Struct | `FStructProperty` | Recurse with same pattern |
| Primitive (int, float, bool) | `FIntProperty`, `FFloatProperty`, `FBoolProperty` | `*static_cast<int32*>(addr)` |
| Array | `FArrayProperty` | `FScriptArrayHelper` |

---

## Debugging Tips

### Log the Struct Type
```cpp
const UScriptStruct* StoredStructType = InstancedStruct->GetScriptStruct();
UE_LOG(LogTemp, Log, TEXT("InstancedStruct contains: %s"),
    StoredStructType ? *StoredStructType->GetName() : TEXT("NULL"));
```

### Log All Properties
```cpp
for (TFieldIterator<FProperty> PropIt(StoredStructType); PropIt; ++PropIt)
{
    FProperty* Prop = *PropIt;
    UE_LOG(LogTemp, Log, TEXT("  Property: %s (%s)"),
        *Prop->GetName(), *Prop->GetClass()->GetName());
}
```

---

## Common Failure Cases

### 1. "Property not found" when using exact name
**Problem:** Using `FindPropertyByName("Forward")` fails because actual name is `Forward_31_GUID`
**Solution:** Use `TFieldIterator` + `StartsWith()`

### 2. GetPtr<T>() returns nullptr
**Problem:** Blueprint struct != C++ struct at runtime
**Solution:** Use reflection, don't rely on type matching

### 3. PropertyValue is nullptr
**Problem:** Soft references not loaded
**Solution:** Use `LoadSynchronous()` for `TSoftObjectPtr`

---

## Real-World Example: SLFActionDodge

**File:** `Source/SLFConversion/Blueprints/Actions/SLFActionDodge.cpp`

This action extracts `DodgeMontages` (Forward, Backstep, Left, Right animations) from a Blueprint data asset's `RelevantData` FInstancedStruct:

```cpp
void USLFActionDodge::ExecuteAction_Implementation()
{
    // ... validation ...

    if (Action)
    {
        // C++ data assets work directly
        if (UPDA_Action* ActionData = Cast<UPDA_Action>(Action))
        {
            if (const FSLFDodgeMontages* MontageData = ActionData->RelevantData.GetPtr<FSLFDodgeMontages>())
            {
                DodgeMontages = *MontageData;
            }
        }
        else
        {
            // Blueprint data assets need reflection
            // (see full pattern above)
        }
    }

    UAnimMontage* MontageToPlay = GetDirectionalDodgeMontage();
    if (MontageToPlay)
    {
        Character->PlayAnimMontage(MontageToPlay);
    }
}
```

---

## When to Use This Pattern

Use this pattern when:
1. Data is stored in `FInstancedStruct`
2. The struct type is defined in Blueprint (not C++ USTRUCT)
3. You need to access properties at runtime from C++
4. Property names may have GUID suffixes

---

## Checklist for New Actions

When implementing a new Action that uses `RelevantData`:

- [ ] Check if `Action` can be cast to `UPDA_Action`
- [ ] If cast fails, Action is Blueprint-derived
- [ ] Use `FindPropertyByName("RelevantData")` to get the property
- [ ] Verify it's an `FInstancedStruct` via `FStructProperty->Struct->GetFName()`
- [ ] Get struct type via `GetScriptStruct()` for logging
- [ ] Use `TFieldIterator` with prefix matching for properties
- [ ] Handle both `FObjectProperty` and `FSoftObjectProperty`
- [ ] Add debug logging for troubleshooting
