"""
Check Blueprint Compilation Errors

This script loads all Blueprints and checks for compilation errors.
"""

import unreal

def check_blueprint_compile_errors():
    print("=" * 60)
    print("Blueprint Compilation Error Check")
    print("=" * 60)

    # Get asset registry
    asset_registry = unreal.AssetRegistryHelpers.get_asset_registry()

    # Find all Blueprint assets
    blueprint_filter = unreal.ARFilter(
        class_names=["Blueprint", "WidgetBlueprint", "AnimBlueprint"],
        package_paths=["/Game/SoulslikeFramework"],
        recursive_paths=True
    )

    assets = asset_registry.get_assets(blueprint_filter)
    print(f"Found {len(assets)} Blueprint assets to check\n")

    errors = []
    warnings = []

    for asset_data in assets:
        asset_path = str(asset_data.package_name) + "." + str(asset_data.asset_name)

        try:
            # Load the asset
            asset = unreal.EditorAssetLibrary.load_asset(asset_path)

            if asset is None:
                errors.append(f"LOAD FAILED: {asset_path}")
                continue

            # Check if it's a Blueprint
            if isinstance(asset, unreal.Blueprint):
                # Try to compile
                bp = asset

                # Check compile status
                status = bp.get_editor_property("status")

                if status == unreal.BlueprintStatus.BS_ERROR:
                    errors.append(f"COMPILE ERROR: {asset_path}")
                elif status == unreal.BlueprintStatus.BS_UP_TO_DATE_WITH_WARNINGS:
                    warnings.append(f"WARNING: {asset_path}")

        except Exception as e:
            errors.append(f"EXCEPTION: {asset_path} - {str(e)}")

    # Print results
    print("\n" + "=" * 60)
    print("RESULTS")
    print("=" * 60)

    if errors:
        print(f"\n=== ERRORS ({len(errors)}) ===")
        for err in errors:
            print(f"  {err}")
    else:
        print("\nNo compilation errors found!")

    if warnings:
        print(f"\n=== WARNINGS ({len(warnings)}) ===")
        for warn in warnings[:20]:  # Limit to first 20
            print(f"  {warn}")
        if len(warnings) > 20:
            print(f"  ... and {len(warnings) - 20} more warnings")

    print("\n" + "=" * 60)
    print(f"Total: {len(assets)} Blueprints checked, {len(errors)} errors, {len(warnings)} warnings")
    print("=" * 60)

if __name__ == "__main__":
    check_blueprint_compile_errors()
