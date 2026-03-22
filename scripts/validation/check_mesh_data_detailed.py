# check_mesh_data_detailed.py
# Detailed check of mesh data assets

import unreal

def log(msg):
    print(f"[MeshCheck] {msg}")
    unreal.log_warning(f"[MeshCheck] {msg}")

def check_asset_properties(asset, asset_name):
    log(f"\n{'='*60}")
    log(f"Asset: {asset_name}")
    log(f"Class: {asset.get_class().get_name()}")

    # Get generated class if it's a blueprint
    gen_class = None
    if hasattr(asset, 'generated_class'):
        gen_class = asset.generated_class()
        if gen_class:
            log(f"Generated Class: {gen_class.get_name()}")

    # Get CDO
    cdo = None
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
    elif hasattr(asset, 'get_default_object'):
        cdo = asset

    if cdo:
        log(f"CDO found: {cdo.get_name()}")

        # Try various property names (snake_case for Python)
        props_to_check = [
            'head_mesh', 'upper_body_mesh', 'arms_mesh', 'lower_body_mesh',
            'default_mesh_asset', 'selected_base_class', 'character_class_name'
        ]

        for prop in props_to_check:
            try:
                val = cdo.get_editor_property(prop)
                if val is not None:
                    # If it's an object, get more details
                    if hasattr(val, 'get_path_name'):
                        log(f"  {prop}: {val.get_path_name()}")
                    elif hasattr(val, 'get_name'):
                        log(f"  {prop}: {val.get_name()}")
                    else:
                        log(f"  {prop}: {val}")
            except Exception as e:
                pass  # Property doesn't exist on this asset

def main():
    log("=" * 80)
    log("DETAILED MESH DATA CHECK")
    log("=" * 80)

    # Check PDA_DefaultMeshData
    asset_path = "/Game/SoulslikeFramework/Data/PDA_DefaultMeshData"
    asset = unreal.load_asset(asset_path)
    if asset:
        check_asset_properties(asset, "PDA_DefaultMeshData")
    else:
        log(f"[ERROR] Could not load: {asset_path}")

    # Check DA_QuinnMeshDefault and DA_MannyMeshDefault
    for name in ['DA_QuinnMeshDefault', 'DA_MannyMeshDefault']:
        path = f"/Game/SoulslikeFramework/Data/BaseCharacters/{name}"
        asset = unreal.load_asset(path)
        if asset:
            check_asset_properties(asset, name)

    # Check DA_Quinn and DA_Manny (character info)
    for name in ['DA_Quinn', 'DA_Manny']:
        path = f"/Game/SoulslikeFramework/Data/BaseCharacters/{name}"
        asset = unreal.load_asset(path)
        if asset:
            check_asset_properties(asset, name)

    # Check GameInstance default for SelectedBaseClass
    gi_path = "/Game/SoulslikeFramework/Blueprints/Framework/GI_SoulslikeFramework"
    gi_bp = unreal.load_asset(gi_path)
    if gi_bp:
        check_asset_properties(gi_bp, "GI_SoulslikeFramework")

    log("\n" + "=" * 80)
    log("CHECK COMPLETE")
    log("=" * 80)

if __name__ == "__main__":
    main()
