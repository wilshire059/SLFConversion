# check_default_mesh_data.py
# Compare PDA_DefaultMeshData between projects

import unreal

def log(msg):
    print(f"[MeshCheck] {msg}")
    unreal.log_warning(f"[MeshCheck] {msg}")

def main():
    log("=" * 80)
    log("DEFAULT MESH DATA CHECK")
    log("=" * 80)

    asset_path = "/Game/SoulslikeFramework/Data/PDA_DefaultMeshData"
    asset = unreal.load_asset(asset_path)

    if not asset:
        log(f"[ERROR] Could not load: {asset_path}")
        return

    log(f"Asset: {asset.get_name()}")
    log(f"Class: {asset.get_class().get_name()}")
    log(f"Parent Class: {asset.get_class().get_super_class().get_name() if asset.get_class().get_super_class() else 'None'}")

    # Check for mesh properties
    log("\n--- Checking Mesh Properties ---")

    mesh_props = ['head_mesh', 'upper_body_mesh', 'arms_mesh', 'lower_body_mesh']
    for prop in mesh_props:
        try:
            val = asset.get_editor_property(prop)
            log(f"{prop}: {val}")
        except Exception as e:
            log(f"{prop}: [ERROR] {e}")

    # Also check GameInstance SelectedBaseClass
    log("\n--- Checking GameInstance SelectedBaseClass ---")
    gi_path = "/Game/SoulslikeFramework/Blueprints/Framework/GI_SoulslikeFramework"
    gi_bp = unreal.load_asset(gi_path)
    if gi_bp:
        log(f"GameInstance: {gi_bp.get_name()}")
        log(f"Class: {gi_bp.get_class().get_name()}")

        # Try to get default object and check SelectedBaseClass
        gen_class = gi_bp.generated_class() if hasattr(gi_bp, 'generated_class') else None
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                try:
                    selected = cdo.get_editor_property('selected_base_class')
                    log(f"SelectedBaseClass (CDO): {selected}")
                except Exception as e:
                    log(f"SelectedBaseClass (CDO): [ERROR] {e}")

    # Check DA_Quinn and DA_Manny DefaultMeshAsset
    log("\n--- Checking Character Info Assets ---")
    for char_name in ['DA_Quinn', 'DA_Manny']:
        char_path = f"/Game/SoulslikeFramework/Data/BaseCharacters/{char_name}"
        char_asset = unreal.load_asset(char_path)
        if char_asset:
            log(f"\n{char_name}:")
            log(f"  Class: {char_asset.get_class().get_name()}")
            try:
                mesh_asset = char_asset.get_editor_property('default_mesh_asset')
                log(f"  DefaultMeshAsset: {mesh_asset}")
                if mesh_asset:
                    log(f"    -> Asset Name: {mesh_asset.get_name() if mesh_asset else 'None'}")
            except Exception as e:
                log(f"  DefaultMeshAsset: [ERROR] {e}")

if __name__ == "__main__":
    main()
