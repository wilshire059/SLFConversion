"""Count Blueprint compilation errors"""
import unreal

# Get all Blueprints in the project
asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()
bp_filter = unreal.ARFilter()
bp_filter.class_names = ["Blueprint"]
bp_filter.recursive_paths = True
bp_filter.package_paths = ["/Game/SoulslikeFramework"]

assets = asset_registry.get_assets(bp_filter)
print(f"Found {len(assets)} Blueprints in SoulslikeFramework")

# Count errors by loading and checking compile status
error_count = 0
warning_count = 0
success_count = 0

error_bps = []

for i, asset_data in enumerate(assets):
    if i > 50:  # Limit for quick check
        break
    bp_path = str(asset_data.package_name)
    bp = unreal.load_asset(bp_path)
    if bp and hasattr(bp, 'generated_class'):
        gc = bp.generated_class()
        if gc:
            # Blueprint loaded successfully
            success_count += 1
        else:
            error_count += 1
            error_bps.append(bp_path)

print("=" * 60)
print(f"BLUEPRINT COMPILE STATUS (first 50)")
print("=" * 60)
print(f"Success: {success_count}")
print(f"Errors:  {error_count}")
if error_bps:
    print("Error Blueprints:")
    for bp in error_bps[:10]:
        print(f"  - {bp}")
print("=" * 60)
