"""
Test to understand UE5 struct naming behavior.
Run with: UnrealEditor-Cmd.exe -run=pythonscript -script="test_struct_names.py"
"""
import unreal

print("=" * 60)
print("STRUCT NAME TEST")
print("=" * 60)

# Test finding structs with different names
test_names = [
    "FSLFDialogRequirement",
    "SLFDialogRequirement",
    "/Script/SLFConversion.FSLFDialogRequirement",
    "/Script/SLFConversion.SLFDialogRequirement",
    "FStatInfo",
    "StatInfo",
    "/Script/SLFConversion.FStatInfo",
    "/Script/SLFConversion.StatInfo",
    "FSLFSkeletalMeshData",
    "SLFSkeletalMeshData",
    "/Script/SLFConversion.FSLFSkeletalMeshData",
    "/Script/SLFConversion.SLFSkeletalMeshData",
]

for name in test_names:
    try:
        struct = unreal.find_object(None, name)
        if struct:
            print(f"[FOUND] {name} -> {struct.get_name()}")
        else:
            print(f"[NOT FOUND] {name}")
    except Exception as e:
        print(f"[ERROR] {name}: {e}")

# Try to find all script structs in our module
print("\n" + "=" * 60)
print("ALL SCRIPT STRUCTS IN /Script/SLFConversion")
print("=" * 60)

# Let's check the actual registered struct names
try:
    # Get registry
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()

    # Look at specific known structs
    known_structs = [
        "/Script/SLFConversion.SLFDialogRequirement",
        "/Script/SLFConversion.FSLFDialogRequirement",
    ]

    for path in known_structs:
        data = asset_registry.get_asset_by_object_path(path)
        if data.is_valid():
            print(f"[REGISTRY] {path}: VALID")
        else:
            print(f"[REGISTRY] {path}: NOT VALID")

except Exception as e:
    print(f"Asset registry error: {e}")

# Try using StaticFindObject
print("\n" + "=" * 60)
print("STATIC FIND OBJECT TEST")
print("=" * 60)

# Import the EditorAssetLibrary
try:
    eal = unreal.EditorAssetLibrary

    # Find a Blueprint that uses these structs
    bp_path = "/Game/SoulslikeFramework/Data/Dialog/PDA_Dialog"
    if eal.does_asset_exist(bp_path):
        asset = eal.load_asset(bp_path)
        if asset:
            print(f"Loaded: {bp_path}")
            print(f"  Class: {asset.get_class().get_name()}")

            # Try to get the DialogEntries property
            try:
                entries = asset.get_editor_property('dialog_entries')
                print(f"  DialogEntries: {entries}")
            except Exception as e:
                print(f"  DialogEntries error: {e}")

except Exception as e:
    print(f"EditorAssetLibrary error: {e}")

print("\nTest complete!")
