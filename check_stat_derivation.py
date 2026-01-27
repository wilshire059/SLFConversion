"""
Check if primary stats have StatsToAffect configured for derivation.
This would explain where Attack Power, Defense Power values come from.
"""
import unreal
import json

# Primary stats that should affect secondary stats
PRIMARY_STATS = [
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Vigor",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Strength",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Dexterity",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Intelligence",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Faith",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Mind",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Endurance",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Arcane",
]

def check_stat_derivation():
    print("=" * 80)
    print("CHECKING STAT DERIVATION (StatsToAffect)")
    print("=" * 80)

    for stat_path in PRIMARY_STATS:
        stat_name = stat_path.split('/')[-1]
        print(f"\n--- {stat_name} ---")

        bp = unreal.EditorAssetLibrary.load_asset(stat_path)
        if not bp:
            print(f"  ERROR: Could not load")
            continue

        gen_class = bp.generated_class()
        if not gen_class:
            print(f"  ERROR: No generated class")
            continue

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            print(f"  ERROR: No CDO")
            continue

        # Try to export as text to see properties
        export_path = f"C:/scripts/SLFConversion/migration_cache/stat_text/{stat_name}.txt"

        export_task = unreal.AssetExportTask()
        export_task.object = bp
        export_task.filename = export_path
        export_task.automated = True
        export_task.prompt = False
        export_task.replace_identical = True

        try:
            success = unreal.Exporter.run_asset_export_task(export_task)
            if success:
                print(f"  Exported to: {export_path}")
        except Exception as e:
            print(f"  Export failed: {e}")

        # Also try to inspect via UHT reflection
        print(f"  Class: {gen_class.get_name()}")
        print(f"  Parent: {gen_class.get_super_class().get_name() if gen_class.get_super_class() else 'None'}")

    print("\n" + "=" * 80)
    print("Done. Check migration_cache/stat_text/*.txt for detailed exports.")
    print("=" * 80)

if __name__ == "__main__":
    import os
    os.makedirs("C:/scripts/SLFConversion/migration_cache/stat_text", exist_ok=True)
    check_stat_derivation()
