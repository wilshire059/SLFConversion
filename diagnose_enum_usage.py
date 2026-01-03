# diagnose_enum_usage.py
# Finds all uses of E_ValueType in Blueprints and reports them
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\diagnose_enum_usage.py").read())

import unreal

def find_enum_asset():
    """Find the E_ValueType enum asset."""
    enum_path = "/Game/SoulslikeFramework/Enums/E_ValueType"
    asset = unreal.EditorAssetLibrary.load_asset(enum_path)
    if asset:
        print(f"Found E_ValueType enum: {asset}")
        # Try to get enum values
        try:
            enum_obj = unreal.UserDefinedEnum.cast(asset)
            if enum_obj:
                print(f"  Type: UserDefinedEnum")
                # Get display names
                names = unreal.EnumEditorLibrary.get_display_name_map(enum_obj)
                print(f"  Values: {names}")
        except Exception as e:
            print(f"  Could not get enum details: {e}")
    return asset

def find_referencers(asset_path):
    """Find all assets that reference the given asset."""
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()

    # Get the asset data
    asset_data = asset_registry.get_asset_by_object_path(asset_path)
    if not asset_data:
        print(f"Could not find asset: {asset_path}")
        return []

    # Find referencers
    referencers = asset_registry.get_referencers(asset_data.package_name)
    return referencers

def main():
    print("\n" + "="*70)
    print("  E_ValueType Usage Diagnostic")
    print("="*70)

    # Find the enum
    print("\n[1] Finding E_ValueType enum...")
    enum_asset = find_enum_asset()

    if not enum_asset:
        print("ERROR: Could not find E_ValueType enum!")
        return

    # Find referencers
    print("\n[2] Finding assets that reference E_ValueType...")
    enum_path = "/Game/SoulslikeFramework/Enums/E_ValueType.E_ValueType"
    referencers = find_referencers(enum_path)

    print(f"\nFound {len(referencers)} referencing packages:")
    for ref in referencers:
        print(f"  - {ref}")

    # Also try the asset registry dependency search
    print("\n[3] Checking known Blueprints for E_ValueType usage...")

    known_blueprints = [
        "/Game/SoulslikeFramework/Data/Stats/B_Stat",
        "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",
        "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_CombatManager",
        "/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager",
        "/Game/SoulslikeFramework/Blueprints/Components/AC_EquipmentManager",
        "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager",
        "/Game/SoulslikeFramework/Widgets/HUD/W_HUD",
    ]

    for bp_path in known_blueprints:
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if bp:
            print(f"  [OK] {bp_path.split('/')[-1]}")
        else:
            print(f"  [--] {bp_path.split('/')[-1]} (not found)")

    print("\n" + "="*70)
    print("  Diagnostic Complete")
    print("="*70)
    print("\nTo see the C++ enum that will replace E_ValueType:")
    print("  ESLFValueType::CurrentValue (0)")
    print("  ESLFValueType::MaxValue (1)")
    print("="*70)

if __name__ == "__main__" or True:
    main()
