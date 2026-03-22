# apply_default_mesh_data.py
# Set mesh properties on DA_QuinnMeshDefault and DA_MannyMeshDefault

import unreal

def log(msg):
    print(f"[MeshData] {msg}")
    unreal.log_warning(f"[MeshData] {msg}")

# Mesh path mappings
QUINN_MESHES = {
    'head_mesh': '/Game/SoulslikeFramework/Meshes/SKM/SKM_QuinnCape_Head.SKM_QuinnCape_Head',
    'upper_body_mesh': '/Game/SoulslikeFramework/Meshes/SKM/SKM_QuinnCape_UpperBody.SKM_QuinnCape_UpperBody',
    'arms_mesh': '/Game/SoulslikeFramework/Meshes/SKM/SKM_Quinn_Arms.SKM_Quinn_Arms',
    'lower_body_mesh': '/Game/SoulslikeFramework/Meshes/SKM/SKM_QuinnCape_LowerBody.SKM_QuinnCape_LowerBody',
}

MANNY_MESHES = {
    'head_mesh': '/Game/SoulslikeFramework/Meshes/SKM/SKM_MannyHead.SKM_MannyHead',
    'upper_body_mesh': '/Game/SoulslikeFramework/Meshes/SKM/SKM_MannyUpperBody.SKM_MannyUpperBody',
    'arms_mesh': '/Game/SoulslikeFramework/Meshes/SKM/SKM_MannyArms.SKM_MannyArms',
    'lower_body_mesh': '/Game/SoulslikeFramework/Meshes/SKM/SKM_MannyLowerBody.SKM_MannyLowerBody',
}

def apply_mesh_data(asset_path, mesh_map, asset_name):
    log(f"\n--- Applying meshes to {asset_name} ---")

    asset = unreal.load_asset(asset_path)
    if not asset:
        log(f"  [ERROR] Could not load: {asset_path}")
        return False

    log(f"  Asset loaded: {asset.get_name()}")
    log(f"  Class: {asset.get_class().get_name()}")

    success = 0
    for prop_name, mesh_path in mesh_map.items():
        try:
            # Load the skeletal mesh
            mesh = unreal.load_asset(mesh_path)
            if not mesh:
                log(f"  [ERROR] Could not load mesh: {mesh_path}")
                continue

            # Set the property using soft object reference
            asset.set_editor_property(prop_name, mesh)
            log(f"  OK: {prop_name} = {mesh.get_name()}")
            success += 1
        except Exception as e:
            log(f"  [ERROR] {prop_name}: {e}")

    # Save the asset
    if success > 0:
        try:
            unreal.EditorAssetLibrary.save_asset(asset_path)
            log(f"  SAVED: {asset_path}")
        except Exception as e:
            log(f"  [ERROR] Save failed: {e}")

    return success == len(mesh_map)

def set_gameinstance_selected_class():
    """Set GameInstance's SelectedBaseClass to DA_Quinn"""
    log("\n--- Setting GameInstance SelectedBaseClass ---")

    gi_path = "/Game/SoulslikeFramework/Blueprints/Framework/GI_SoulslikeFramework"
    gi_bp = unreal.load_asset(gi_path)
    if not gi_bp or not hasattr(gi_bp, 'generated_class'):
        log("  [ERROR] Could not load GameInstance Blueprint")
        return

    gen_class = gi_bp.generated_class()
    if not gen_class:
        log("  [ERROR] No generated class")
        return

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        log("  [ERROR] Could not get CDO")
        return

    # Load DA_Quinn
    quinn_path = "/Game/SoulslikeFramework/Data/BaseCharacters/DA_Quinn"
    quinn = unreal.load_asset(quinn_path)
    if not quinn:
        log(f"  [ERROR] Could not load: {quinn_path}")
        return

    try:
        cdo.set_editor_property('selected_base_class', quinn)
        log(f"  OK: selected_base_class = {quinn.get_name()}")

        # Mark blueprint as modified and save
        unreal.EditorAssetLibrary.save_asset(gi_path)
        log(f"  SAVED: {gi_path}")
    except Exception as e:
        log(f"  [ERROR] {e}")

def main():
    log("=" * 80)
    log("APPLYING DEFAULT MESH DATA")
    log("=" * 80)

    # Apply to DA_QuinnMeshDefault
    apply_mesh_data(
        "/Game/SoulslikeFramework/Data/BaseCharacters/DA_QuinnMeshDefault",
        QUINN_MESHES,
        "DA_QuinnMeshDefault"
    )

    # Apply to DA_MannyMeshDefault
    apply_mesh_data(
        "/Game/SoulslikeFramework/Data/BaseCharacters/DA_MannyMeshDefault",
        MANNY_MESHES,
        "DA_MannyMeshDefault"
    )

    # Set GameInstance SelectedBaseClass to DA_Quinn
    set_gameinstance_selected_class()

    log("\n" + "=" * 80)
    log("COMPLETE - Run in PIE to verify Quinn mesh is now used")
    log("=" * 80)

if __name__ == "__main__":
    main()
