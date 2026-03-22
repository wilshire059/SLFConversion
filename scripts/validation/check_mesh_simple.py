# check_mesh_simple.py
# Simple check of mesh data assets

import unreal

def log(msg):
    unreal.log_warning(f"[MeshInfo] {msg}")

def main():
    log("=" * 60)
    log("MESH DATA CHECK")
    log("=" * 60)

    # Skip hierarchy check - just check mesh data

    # Check DA_QuinnMeshDefault
    log("--- DA_QuinnMeshDefault ---")
    quinn_mesh = unreal.load_asset("/Game/SoulslikeFramework/Data/BaseCharacters/DA_QuinnMeshDefault")
    if quinn_mesh:
        for prop in ['head_mesh', 'upper_body_mesh', 'arms_mesh', 'lower_body_mesh']:
            try:
                val = quinn_mesh.get_editor_property(prop)
                log(f"  {prop}: {val}")
            except:
                log(f"  {prop}: [not found]")
    else:
        log("  [Asset not found]")

    # Check DA_MannyMeshDefault
    log("--- DA_MannyMeshDefault ---")
    manny_mesh = unreal.load_asset("/Game/SoulslikeFramework/Data/BaseCharacters/DA_MannyMeshDefault")
    if manny_mesh:
        for prop in ['head_mesh', 'upper_body_mesh', 'arms_mesh', 'lower_body_mesh']:
            try:
                val = manny_mesh.get_editor_property(prop)
                log(f"  {prop}: {val}")
            except:
                log(f"  {prop}: [not found]")
    else:
        log("  [Asset not found]")

    # Check GameInstance SelectedBaseClass
    log("--- GameInstance SelectedBaseClass ---")
    gi = unreal.load_asset("/Game/SoulslikeFramework/Blueprints/Framework/GI_SoulslikeFramework")
    if gi and hasattr(gi, 'generated_class'):
        gen_class = gi.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                try:
                    selected = cdo.get_editor_property('selected_base_class')
                    if selected:
                        log(f"  selected_base_class: {selected.get_name()}")
                    else:
                        log(f"  selected_base_class: None")
                except Exception as e:
                    log(f"  selected_base_class: [ERROR] {e}")

    log("=" * 60)

if __name__ == "__main__":
    main()
