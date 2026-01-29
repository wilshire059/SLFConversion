# Creating Spell Effects from Scratch

## Overview

This skill documents the complete process for creating new spell/projectile effects in the SoulslikeFramework project. This includes:
1. C++ spell classes (projectiles, AOE, beams, etc.)
2. Data assets for inventory/equipment integration
3. Proper class hierarchy for Cast<> compatibility

---

## CRITICAL: Data Asset Class Requirements

**The most important lesson learned:**

Data assets MUST be created using the **Blueprint class** (`PDA_Item_C`), NOT the direct C++ class (`PDA_Item`/`UPDA_Item`).

### Why This Matters

The inventory system uses `Cast<UPDA_Item>(Asset)` to filter items. While both should theoretically work:
- **Blueprint class** (`PDA_Item_C`): Works correctly with Cast<>
- **C++ class** (`PDA_Item`): May fail Cast<> in certain scenarios

### Working vs Non-Working Pattern

| Approach | Class | Path | Works? |
|----------|-------|------|--------|
| Duplicate from existing | `PDA_Item_C` | `/Game/.../PDA_Item.PDA_Item_C` | **YES** |
| Create via Python API | `PDA_Item` | `/Script/SLFConversion.PDA_Item` | **NO** |

---

## Step 1: Create C++ Spell Classes

### Base Spell Projectile Class

All projectile spells inherit from `ASLFSpellProjectile`:

```cpp
// SLFSpellProjectile.h
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFSpellProjectile : public AActor
{
    GENERATED_BODY()

public:
    ASLFSpellProjectile();

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* CollisionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UProjectileMovementComponent* ProjectileMovement;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComponent;

    // Configuration
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spell|Damage")
    float BaseDamage = 50.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spell|Movement")
    float ProjectileSpeed = 2000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spell|Visuals")
    FLinearColor SpellColor = FLinearColor::White;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spell|Visuals")
    float MeshScale = 1.0f;

    // Caster reference
    UPROPERTY(BlueprintReadWrite, Category = "Spell")
    AActor* SpellCaster;

protected:
    virtual void BeginPlay() override;
    virtual void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    void ApplySpellEffects(AActor* Target);
    void CreateVisualMesh();
};
```

### Specific Spell Types

Create subclasses for each spell type:

```cpp
// SLFFireballProjectile.h
UCLASS()
class SLFCONVERSION_API ASLFFireballProjectile : public ASLFSpellProjectile
{
    GENERATED_BODY()

public:
    ASLFFireballProjectile();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fireball")
    float ExplosionRadius = 200.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Fireball")
    float BurnDamagePerSecond = 10.0f;

protected:
    virtual void OnProjectileHit(...) override;
    void CreateExplosion(const FVector& Location);
};
```

### Constructor Pattern (Mesh-Based VFX)

```cpp
ASLFFireballProjectile::ASLFFireballProjectile()
{
    // Set spell-specific defaults
    BaseDamage = 75.0f;
    ProjectileSpeed = 1500.0f;
    SpellColor = FLinearColor(1.0f, 0.4f, 0.0f, 1.0f); // Orange
    MeshScale = 0.5f;

    // Fireball-specific
    ExplosionRadius = 200.0f;
    BurnDamagePerSecond = 10.0f;
}
```

### Mesh-Based Visual Effects

The spell uses runtime-generated meshes for visual effects:

```cpp
void ASLFSpellProjectile::CreateVisualMesh()
{
    if (!MeshComponent) return;

    // Create procedural sphere mesh or use engine primitive
    UStaticMesh* SphereMesh = LoadObject<UStaticMesh>(nullptr,
        TEXT("/Engine/BasicShapes/Sphere.Sphere"));

    if (SphereMesh)
    {
        MeshComponent->SetStaticMesh(SphereMesh);
        MeshComponent->SetWorldScale3D(FVector(MeshScale));

        // Create dynamic material with spell color
        UMaterialInstanceDynamic* DynMat = MeshComponent->CreateDynamicMaterialInstance(0);
        if (DynMat)
        {
            DynMat->SetVectorParameterValue(TEXT("BaseColor"), SpellColor);
            DynMat->SetScalarParameterValue(TEXT("Emissive"), 5.0f);
        }
    }
}
```

---

## Step 2: Create Data Assets (CRITICAL)

### Method: Duplicate from Working Item

**DO NOT use `asset_tools.create_asset()` with C++ class.**

Instead, duplicate from an existing working item:

```python
# recreate_spells_from_blueprint.py

import unreal

# Template - MUST be an existing working item with PDA_Item_C class
TEMPLATE_PATH = "/Game/SoulslikeFramework/Data/Items/DA_MagicSpell.DA_MagicSpell"
SPELLS_FOLDER = "/Game/SoulslikeFramework/Data/Items/Spells"

SPELLS = [
    {
        "name": "DA_FireballSpell",
        "display_name": "Fireball",
        "description": "Hurls a blazing fireball that explodes on impact.",
        "cpp_class": "/Script/SLFConversion.SLFFireballProjectile",
    },
    # ... more spells
]

def create_spell_data_assets():
    # Load template (MUST be PDA_Item_C class)
    template = unreal.EditorAssetLibrary.load_asset(TEMPLATE_PATH)
    if not template:
        print("ERROR: Could not load template")
        return

    # Verify template class
    template_class = template.get_class()
    print(f"Template class: {template_class.get_name()}")  # Should be "PDA_Item_C"

    for spell in SPELLS:
        spell_path = f"{SPELLS_FOLDER}/{spell['name']}"

        # Delete existing if present (must delete file on disk first)
        # rm Content/.../Spells/*.uasset

        # CRITICAL: Duplicate from template (inherits PDA_Item_C class)
        result = unreal.EditorAssetLibrary.duplicate_asset(TEMPLATE_PATH, spell_path)
        if not result:
            print(f"ERROR: Failed to duplicate {spell['name']}")
            continue

        # Load duplicated asset
        asset = unreal.EditorAssetLibrary.load_asset(f"{spell_path}.{spell['name']}")

        # Configure item information
        item_info = asset.get_editor_property("item_information")
        item_info.set_editor_property("display_name", spell['display_name'])
        item_info.set_editor_property("short_description", spell['description'])

        # Set C++ spell class
        spell_class = unreal.load_class(None, spell['cpp_class'])
        if spell_class:
            item_info.set_editor_property("item_class", spell_class)

        # Category and action_to_trigger inherited from template
        # (Template should have Category=Tools, Usable=True)

        asset.set_editor_property("item_information", item_info)

        # Save
        unreal.EditorAssetLibrary.save_asset(f"{spell_path}.{spell['name']}")
        print(f"Created: {spell['name']}")
```

### Important: Delete Files on Disk First

Before running the duplication script, delete existing assets from disk:

```bash
rm -f Content/SoulslikeFramework/Data/Items/Spells/*.uasset
```

The Python `delete_asset()` function may not fully clear assets, causing duplication to fail.

### Verification Script

Always verify the created assets have correct class:

```python
def verify_spell_class(asset_path):
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if asset:
        class_name = asset.get_class().get_name()
        class_path = asset.get_class().get_path_name()

        # MUST be PDA_Item_C, NOT PDA_Item
        if class_name == "PDA_Item_C":
            print(f"OK: {asset_path} -> {class_name}")
            return True
        else:
            print(f"ERROR: {asset_path} -> {class_name} (expected PDA_Item_C)")
            return False
    return False
```

---

## Step 3: Add Spells to Inventory

### Option A: In InventoryManagerComponent::BeginPlay()

```cpp
void UInventoryManagerComponent::BeginPlay()
{
    Super::BeginPlay();

    // Add spell to inventory
    static const TCHAR* SpellPaths[] = {
        TEXT("/Game/SoulslikeFramework/Data/Items/Spells/DA_FireballSpell.DA_FireballSpell"),
        TEXT("/Game/SoulslikeFramework/Data/Items/Spells/DA_IceShardSpell.DA_IceShardSpell"),
        // ... more spells
    };

    for (const TCHAR* SpellPath : SpellPaths)
    {
        FSoftObjectPath Path(SpellPath);
        if (UObject* LoadedSpell = Path.TryLoad())
        {
            if (UDataAsset* SpellData = Cast<UDataAsset>(LoadedSpell))
            {
                AddItem(SpellData, 99, false); // 99 charges, no loot UI
            }
        }
    }
}
```

### Option B: Via Data Table or Starting Inventory

Add to PC_SoulslikeFramework StartingInventory array property.

---

## Step 4: Equipment Slot Filtering

Spells appear in Tool slots because:

```cpp
// In GetItemsForEquipmentSlot_Implementation:
else if (bIsToolSlot)
{
    bMatches = (Category == ESLFItemCategory::Tools) ||
               (SubCategory == ESLFItemSubCategory::Flasks) ||
               (SubCategory == ESLFItemSubCategory::Projectiles);
}
```

Ensure data asset has:
- `ItemInformation.Category.Category = ESLFItemCategory::Tools` (value 1)
- `ItemInformation.Usable = true`

---

## Step 5: Spell Casting Integration

### Action Class (B_Action_UseEquippedTool or similar)

The action spawns the spell projectile:

```cpp
void USLFActionUseEquippedTool::ActivateAbility()
{
    // Get equipped tool from Tool slot
    UPDA_Item* ToolItem = GetEquippedTool();
    if (!ToolItem) return;

    // Get the ItemClass (spell projectile class)
    UClass* SpellClass = ToolItem->ItemInformation.ItemClass;
    if (!SpellClass) return;

    // Spawn the projectile
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = GetOwningActor();
    SpawnParams.Instigator = Cast<APawn>(GetOwningActor());

    FVector SpawnLocation = GetOwningActor()->GetActorLocation() +
        GetOwningActor()->GetActorForwardVector() * 100.0f;
    FRotator SpawnRotation = GetOwningActor()->GetActorRotation();

    ASLFSpellProjectile* Spell = GetWorld()->SpawnActor<ASLFSpellProjectile>(
        SpellClass, SpawnLocation, SpawnRotation, SpawnParams);

    if (Spell)
    {
        Spell->SpellCaster = GetOwningActor();
    }
}
```

---

## Complete Spell Type Checklist

| Spell Type | C++ Class | Base Class | Key Properties |
|------------|-----------|------------|----------------|
| Fireball | ASLFFireballProjectile | ASLFSpellProjectile | ExplosionRadius, BurnDamage |
| Ice Shard | ASLFIceShardProjectile | ASLFSpellProjectile | SlowAmount, PierceCount |
| Lightning Bolt | ASLFLightningBoltProjectile | ASLFSpellProjectile | ChainCount, FastSpeed |
| Holy Orb | ASLFHolyOrbProjectile | ASLFSpellProjectile | HomingStrength |
| Poison Blob | ASLFPoisonBlobProjectile | ASLFSpellProjectile | PoisonDuration, DOT |
| Arcane Missile | ASLFArcaneMissileProjectile | ASLFSpellProjectile | ManaScaling |
| Blackhole | (uses base) | ASLFSpellProjectile | PullForce |
| Energy Beam | ASLFBeamSpell | AActor | BeamLength, TickDamage |
| AOE Explosion | ASLFAOESpell | AActor | Radius, FalloffCurve |
| Magic Shield | ASLFShieldSpell | AActor | AbsorbAmount, Duration |
| Heal | ASLFHealingSpell | AActor | HealPerTick, Duration |
| Summoning Circle | ASLFSummoningCircle | AActor | EffectRadius |

---

## Troubleshooting

### Spells Do Not Appear in Tool Slot Selection

1. **Check data asset class**: Must be `PDA_Item_C`, not `PDA_Item`
2. **Check Category**: Must be `ESLFItemCategory::Tools` (value 1)
3. **Check Usable flag**: Must be `true`
4. **Check Cast<UPDA_Item>**: Add logging to verify cast succeeds

### Cast<UPDA_Item> Fails

**Root Cause**: Data asset created with wrong class.

**Fix**: Recreate by duplicating from working item:
```bash
# Delete existing
rm Content/SoulslikeFramework/Data/Items/Spells/*.uasset

# Run recreate script
UnrealEditor-Cmd.exe Project.uproject -run=pythonscript -script=recreate_spells_from_blueprint.py
```

### Spell Does Not Spawn

1. Check `ItemClass` is set on data asset
2. Verify C++ class is compiled and loadable
3. Check action class spawning logic

---

## Files Reference

| File | Purpose |
|------|---------|
| `Source/SLFConversion/Spells/SLFSpellProjectile.h/cpp` | Base projectile class |
| `Source/SLFConversion/Spells/SLF*Projectile.h/cpp` | Specific spell classes |
| `recreate_spells_from_blueprint.py` | Create data assets correctly |
| `Content/.../Items/Spells/DA_*.uasset` | Spell data assets |
| `InventoryManagerComponent.cpp` | Adds spells to inventory |

---

## Summary

**The Golden Rule**: Always create spell data assets by **duplicating from an existing working item** that uses the Blueprint class `PDA_Item_C`. Never use `create_asset()` with the direct C++ class.

This ensures the data assets have the correct class hierarchy for `Cast<UPDA_Item>` to work in the inventory filtering system.
