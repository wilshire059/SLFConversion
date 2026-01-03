# reconstruct_nodes.py
# Reconstructs all nodes in affected Blueprints to pick up new C++ types
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\reconstruct_nodes.py").read())

import unreal

BLUEPRINTS_TO_FIX = [
    "/Game/SoulslikeFramework/Data/Stats/B_Stat",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect",
    "/Game/SoulslikeFramework/Widgets/HUD/W_HUD",
    "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelUp",
]

def main():
    print("\n" + "="*70)
    print("  Reconstruct All Nodes in Affected Blueprints")
    print("="*70)

    try:
        fixer = unreal.BlueprintFixerLibrary
    except AttributeError:
        print("ERROR: BlueprintFixerLibrary not found!")
        return

    for bp_path in BLUEPRINTS_TO_FIX:
        bp_name = bp_path.split('/')[-1]
        print(f"\n[{bp_name}]")

        asset = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not asset:
            print(f"  ERROR: Could not load")
            continue

        bp = unreal.Blueprint.cast(asset)
        if not bp:
            bp = unreal.WidgetBlueprint.cast(asset)
        if not bp:
            print(f"  ERROR: Not a Blueprint type")
            continue

        # Reconstruct all nodes
        result = fixer.reconstruct_all_nodes(bp)
        print(f"  Reconstructed: {result}")

        # Save
        try:
            unreal.EditorAssetLibrary.save_asset(bp_path)
            print(f"  Saved")
        except Exception as e:
            print(f"  Save warning: {e}")

    print("\n" + "="*70)
    print("  Done! Try compiling Blueprints now.")
    print("="*70)

if __name__ == "__main__" or True:
    main()
