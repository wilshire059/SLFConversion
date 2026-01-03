# find_enumerator_pins.py
# Finds all pins that contain "Enumerator" in their value
# Use this to diagnose what enum values need fixing
#
# Run in Unreal Editor Python console:
#   exec(open(r"C:\scripts\SLFConversion\find_enumerator_pins.py").read())

import unreal

TEST_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Data/Stats/B_Stat",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",
]

def main():
    print("\n" + "="*70)
    print("  FIND ENUMERATOR PIN VALUES")
    print("="*70)

    try:
        fixer = unreal.BlueprintFixerLibrary
        print("BlueprintFixerLibrary found!")
    except AttributeError:
        print("ERROR: BlueprintFixerLibrary not found!")
        return

    # Check if the new function exists
    if not hasattr(fixer, 'get_all_pin_values'):
        print("ERROR: get_all_pin_values function not found! Rebuild C++.")
        return

    for bp_path in TEST_BLUEPRINTS:
        bp_name = bp_path.split('/')[-1]
        print(f"\n{'='*60}")
        print(f"  {bp_name}")
        print(f"{'='*60}")

        asset = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not asset:
            print(f"  ERROR: Could not load asset")
            continue

        bp = unreal.Blueprint.cast(asset)
        if not bp:
            bp = unreal.WidgetBlueprint.cast(asset)
        if not bp:
            print(f"  ERROR: Not a Blueprint type")
            continue

        # Get all pins with values
        all_pins = fixer.get_all_pin_values(bp)

        # Filter for pins containing "Enumerator"
        enumerator_pins = [p for p in all_pins if "Enumerator" in str(p)]

        print(f"\n  Found {len(enumerator_pins)} pins with 'Enumerator' in value:")
        for pin in enumerator_pins:
            print(f"    {pin}")

        if not enumerator_pins:
            print("    None found")

        # Also show some sample pins for context
        print(f"\n  Sample of ALL pins with values ({len(all_pins)} total):")
        for pin in all_pins[:10]:
            print(f"    {pin}")
        if len(all_pins) > 10:
            print(f"    ... and {len(all_pins) - 10} more")

    print("\n" + "="*70)
    print("  DIAGNOSTIC COMPLETE")
    print("="*70)

if __name__ == "__main__" or True:
    main()
