"""
Analyze the impact of struct migration - what assets use which structs.
"""
import unreal
import re

print("=" * 70)
print("STRUCT MIGRATION IMPACT ANALYSIS")
print("=" * 70)

# Collect unique struct mismatches by running a test load
# For now, let's analyze which C++ classes use which struct types

eal = unreal.EditorAssetLibrary

# Check key data asset classes
data_asset_classes = [
    "/Script/SLFConversion.PDA_Item",
    "/Script/SLFConversion.PDA_Action",
    "/Script/SLFConversion.PDA_DefaultMeshData",
    "/Script/SLFConversion.PDA_Dialog",
    "/Script/SLFConversion.PDA_Vendor",
    "/Script/SLFConversion.PDA_DayNight",
    "/Script/SLFConversion.B_Stat",
]

print("\n=== DATA ASSET CLASSES WITH STRUCT PROPERTIES ===")
for class_path in data_asset_classes:
    try:
        cls = unreal.load_class(None, class_path)
        if cls:
            print(f"\n{cls.get_name()}:")
            # List properties (we can't easily filter by struct type in Python)
            cdo = unreal.get_default_object(cls)
            if cdo:
                print(f"  CDO loaded: Yes")
    except Exception as e:
        print(f"{class_path}: Error - {e}")

# Count assets by type that might be affected
print("\n=== ASSET COUNTS BY TYPE ===")

asset_paths_to_check = [
    ("/Game/SoulslikeFramework/Data/Items", "Item Data Assets"),
    ("/Game/SoulslikeFramework/Data/Stats", "Stat Data Assets"),
    ("/Game/SoulslikeFramework/Data/Dialog", "Dialog Data Assets"),
    ("/Game/SoulslikeFramework/Data/Vendor", "Vendor Data Assets"),
    ("/Game/SoulslikeFramework/Data/BaseCharacters", "Character Data Assets"),
    ("/Game/SoulslikeFramework/Widgets", "Widgets"),
    ("/Game/SoulslikeFramework/Blueprints/Components", "Components"),
    ("/Game/SoulslikeFramework/Blueprints/_Characters", "Characters"),
]

total_assets = 0
for path, description in asset_paths_to_check:
    try:
        assets = eal.list_assets(path, recursive=True, include_folder=False)
        count = len(assets)
        total_assets += count
        print(f"  {description}: {count} assets")
    except:
        print(f"  {description}: Error listing")

print(f"\n  TOTAL: {total_assets} assets potentially affected")

# Test if we can actually read struct data after load
print("\n=== STRUCT DATA ACCESSIBILITY TEST ===")

# Try to load a data asset and access its struct property
test_asset = "/Game/SoulslikeFramework/Data/Items/DA_Apple"
if eal.does_asset_exist(test_asset):
    asset = eal.load_asset(test_asset)
    if asset:
        print(f"Loaded: {test_asset}")
        try:
            # Try to access ItemInformation (FSLFItemInfo struct)
            item_info = asset.get_editor_property('item_information')
            print(f"  ItemInformation accessible: {item_info is not None}")
            if item_info:
                # Try to access a nested property
                try:
                    display_name = item_info.get_editor_property('display_name')
                    print(f"  DisplayName: {display_name}")
                except Exception as e:
                    print(f"  DisplayName error: {e}")
        except Exception as e:
            print(f"  ItemInformation error: {e}")

print("\n=== SUMMARY ===")
print("The struct mismatch causes data to NOT load properly.")
print("Any resave would save DEFAULT values, losing the original data.")
print("A proper migration needs to:")
print("  1. Extract data BEFORE type mismatch occurs")
print("  2. Apply correct struct type names")
print("  3. Re-import data with correct types")
