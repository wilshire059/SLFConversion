import unreal
import json

# Check B_DeathCurrency stat defaults in bp_only
stat_path = "/Game/SoulslikeFramework/Data/Stats/Misc/B_DeathCurrency"
print(f"\n=== Checking B_DeathCurrency at {stat_path} ===")

bp = unreal.EditorAssetLibrary.load_asset(stat_path)
if not bp:
    # Try alternative paths
    for alt_path in ["/Game/SoulslikeFramework/Data/Stats/B_DeathCurrency",
                     "/Game/SoulslikeFramework/Data/Stats/Currency/B_DeathCurrency"]:
        bp = unreal.EditorAssetLibrary.load_asset(alt_path)
        if bp:
            print(f"Found at: {alt_path}")
            break

if bp:
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        print(f"CDO class: {type(cdo)}")

        # Get all properties
        for prop in dir(cdo):
            if not prop.startswith('_'):
                try:
                    val = getattr(cdo, prop)
                    if val is not None and not callable(val):
                        print(f"  {prop}: {val}")
                except:
                    pass
else:
    print("B_DeathCurrency not found!")

    # List all stats to find currency
    stats_paths = [
        "/Game/SoulslikeFramework/Data/Stats/Misc",
        "/Game/SoulslikeFramework/Data/Stats"
    ]
    for path in stats_paths:
        print(f"\n=== Assets in {path} ===")
        assets = unreal.EditorAssetLibrary.list_assets(path, recursive=False)
        for asset in assets:
            print(f"  {asset}")

print("\n=== Done ===")
