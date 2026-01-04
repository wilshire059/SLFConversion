# C++ Property Migration Skill

## Purpose
Migrate Blueprint struct properties to C++ UPROPERTY declarations to enable direct C++ access without reflection. This eliminates the need for runtime reflection to access Blueprint struct data with GUID-suffixed property names.

## When to Use
Use this approach when:
- C++ code needs to access data stored in Blueprint struct properties
- The Blueprint struct has GUID-suffixed property names (e.g., `WorldNiagaraSystem_14_EF222BEA43E8FE509F34329013D17763`)
- Reflection-based access is working but is complex and fragile
- You want cleaner, faster, type-safe property access

## The Problem

Blueprint structs store properties with GUID suffixes that change between editor sessions:
```
ItemInformation.WorldPickupInfo.WorldNiagaraSystem_14_EF222BEA43E8FE509F34329013D17763
```

C++ reflection code must iterate through properties and match by prefix, which is:
- Slow (runtime property iteration)
- Fragile (depends on struct layout)
- Complex (nested struct traversal)
- Hard to maintain

## The Solution

1. Add equivalent UPROPERTY declarations to the C++ parent class
2. Reparent the Blueprint to the C++ class
3. Migrate data from Blueprint properties to C++ properties
4. Access C++ properties directly via cast

## Step-by-Step Workflow

### Step 1: Add C++ Property

In the C++ class that will be the new parent (e.g., `UPDA_Item`):

```cpp
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API UPDA_Item : public UPDA_Base
{
    GENERATED_BODY()
public:
    // Mirror of Blueprint's ItemInformation.WorldPickupInfo.WorldNiagaraSystem
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|WorldPickup")
    TSoftObjectPtr<UNiagaraSystem> WorldNiagaraSystem;
};
```

### Step 2: Extract Data from Backup

Before reparenting, extract the existing data. Use `export_text()` on Blueprint structs to get a text representation that includes GUID-suffixed property names:

```python
# extract_data.py
import unreal
import json
import re

def extract_data():
    data = {}
    items_path = "/Game/SoulslikeFramework/Data/Items"
    assets = unreal.EditorAssetLibrary.list_assets(items_path, recursive=True, include_folder=False)

    for asset_path in assets:
        asset_name = asset_path.split("/")[-1].split(".")[0]
        asset = unreal.EditorAssetLibrary.load_asset(asset_path)

        # Get Blueprint struct and export as text
        struct_prop = asset.get_editor_property('ItemInformation')
        export = struct_prop.export_text()

        # Parse with regex - handles GUID suffixes
        match = re.search(r'WorldNiagaraSystem[^=]*="([^"]+)"', export)
        if match:
            data[asset_name] = match.group(1)

    with open("C:/scripts/SLFConversion/extracted_data.json", 'w') as f:
        json.dump(data, f, indent=2)
```

### Step 3: Reparent Blueprint

Use `BlueprintEditorLibrary.reparent_blueprint()`:

```python
# reparent_blueprint.py
import unreal

def reparent():
    # Load the Blueprint
    bp = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Data/PDA_Item")

    # Load C++ class (note: Python uses class name without "U" prefix)
    cpp_class = unreal.load_class(None, "/Script/SLFConversion.PDA_Item")

    # Reparent
    unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)

    # Compile and save
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    unreal.EditorAssetLibrary.save_asset("/Game/SoulslikeFramework/Data/PDA_Item")
```

**Important Python naming convention:**
- C++ `UPDA_Item` → Python `unreal.PDA_Item` (U prefix stripped)
- C++ `UPDA_Base` → Python `unreal.PDA_Base`
- Class path: `/Script/ModuleName.ClassName` (without U prefix)

### Step 4: Apply Data to C++ Properties

```python
# apply_data.py
import unreal
import json

def apply_data():
    with open("C:/scripts/SLFConversion/extracted_data.json", 'r') as f:
        data = json.load(f)

    for asset_name, asset_path_value in data.items():
        item_path = f"/Game/SoulslikeFramework/Data/Items/{asset_name}"
        item = unreal.EditorAssetLibrary.load_asset(item_path)

        # Load the referenced asset
        referenced_asset = unreal.EditorAssetLibrary.load_asset(asset_path_value)

        # Set C++ property (use snake_case for Python)
        item.set_editor_property('world_niagara_system', referenced_asset)

        # Save
        unreal.EditorAssetLibrary.save_asset(item_path)
```

**Property naming in Python:**
- C++ `WorldNiagaraSystem` → Python `world_niagara_system` (snake_case)
- C++ `MyPropertyName` → Python `my_property_name`

### Step 5: Update C++ Consumer Code

```cpp
void ASLFPickupItemBase::SetupWorldNiagara()
{
    UNiagaraComponent* NiagaraComp = FindComponentByClass<UNiagaraComponent>();
    if (!NiagaraComp || !Item) return;

    // Direct C++ access - no reflection needed!
    if (UPDA_Item* ItemData = Cast<UPDA_Item>(Item))
    {
        if (!ItemData->WorldNiagaraSystem.IsNull())
        {
            UNiagaraSystem* System = ItemData->WorldNiagaraSystem.LoadSynchronous();
            if (System)
            {
                NiagaraComp->SetAsset(System);
                NiagaraComp->Activate(true);
                return;
            }
        }
    }

    // Optional: Keep reflection fallback for unmigrated data
    SetupWorldNiagaraViaReflection(NiagaraComp);
}
```

## Complete Script Set

### run_full_migration.ps1
```powershell
# 1. Restore backup (preserves original data)
Remove-Item -Path 'C:\scripts\SLFConversion\Content\*' -Recurse -Force
Copy-Item -Path 'C:\scripts\bp_only\Content\*' -Destination 'C:\scripts\SLFConversion\Content\' -Recurse -Force

# 2. Extract data from Blueprint properties
& "C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" `
  "C:/scripts/SLFConversion/SLFConversion.uproject" `
  -run=pythonscript -script="C:/scripts/SLFConversion/extract_data.py" `
  -stdout -unattended -nosplash

# 3. Reparent Blueprint to C++ class
& "C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" `
  "C:/scripts/SLFConversion/SLFConversion.uproject" `
  -run=pythonscript -script="C:/scripts/SLFConversion/reparent_blueprint.py" `
  -stdout -unattended -nosplash

# 4. Apply extracted data to C++ properties
& "C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" `
  "C:/scripts/SLFConversion/SLFConversion.uproject" `
  -run=pythonscript -script="C:/scripts/SLFConversion/apply_data.py" `
  -stdout -unattended -nosplash
```

## Key Techniques

### 1. export_text() for Blueprint Struct Data
```python
struct_prop = asset.get_editor_property('StructPropertyName')
text = struct_prop.export_text()
# Returns: "(PropertyName_XX_GUID=Value, AnotherProp_YY_GUID=...)"
```

### 2. Regex for GUID-Suffixed Properties
```python
import re
# Match property name with any GUID suffix
match = re.search(r'PropertyName[^=]*="([^"]+)"', text)
value = match.group(1) if match else None
```

### 3. Python Class Name Convention
```python
# C++ UPDA_Item → Python load path
cls = unreal.load_class(None, "/Script/ModuleName.PDA_Item")  # No U prefix!

# Direct module access
cls = unreal.PDA_Item  # Also no U prefix
```

### 4. Python Property Name Convention
```python
# C++ WorldNiagaraSystem → Python snake_case
asset.set_editor_property('world_niagara_system', value)
asset.get_editor_property('world_niagara_system')
```

## Verification

After migration, verify with:
```python
item = unreal.EditorAssetLibrary.load_asset("/Game/.../DA_Apple")
value = item.get_editor_property('world_niagara_system')
print(f"C++ property value: {value.get_name() if value else 'None'}")
```

In PIE, check logs for:
```
[PickupItem] Using C++ UPDA_Item direct access
[PickupItem] Set Niagara System (C++ direct): NS_ItemLoot
```

NOT:
```
[PickupItem] Falling back to reflection-based access
```

## Benefits Over Reflection

| Aspect | Reflection | C++ Direct |
|--------|------------|------------|
| Speed | Slow (runtime iteration) | Fast (compile-time offset) |
| Type Safety | None (void pointers) | Full (compiler checks) |
| Maintainability | Complex nested traversal | Simple property access |
| Debugging | Hard to trace | Easy to inspect |
| Code Size | ~150 lines | ~10 lines |

## Files Created for PDA_Item Migration

| File | Purpose |
|------|---------|
| `extract_niagara_paths.py` | Extract WorldNiagaraSystem from Blueprint structs |
| `reparent_pda_item.py` | Reparent PDA_Item to C++ UPDA_Item |
| `apply_niagara_paths.py` | Apply paths to C++ WorldNiagaraSystem property |
| `niagara_paths.json` | Extracted data (21 items) |
| `SLFPrimaryDataAssets.h` | C++ UPDA_Item with WorldNiagaraSystem UPROPERTY |
| `SLFPickupItemBase.cpp` | Updated to use Cast<UPDA_Item> for direct access |

## Rule: Always Prefer C++ Properties Over Reflection

When C++ needs to access Blueprint data:
1. **DO**: Add UPROPERTY to C++ class, reparent Blueprint, migrate data
2. **DON'T**: Write reflection code to traverse Blueprint struct hierarchies

Reflection should only be used as a temporary fallback during migration, not as the permanent solution.
