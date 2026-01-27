# check_mesh_inheritance.py
# Check inheritance chains for mesh data assets

import unreal

def log(msg):
    print(f"[Inherit] {msg}")
    unreal.log_warning(f"[Inherit] {msg}")

def print_class_hierarchy(cls, indent=0, max_depth=10):
    """Print full class hierarchy"""
    if cls is None or indent > max_depth:
        return
    log(f"{'  '*indent}{cls.get_name()}")
    parent = cls.get_super_class()
    if parent:
        print_class_hierarchy(parent, indent + 1, max_depth)

def main():
    log("=" * 80)
    log("MESH DATA INHERITANCE CHECK")
    log("=" * 80)

    # Check PDA_DefaultMeshData hierarchy
    log("\n--- PDA_DefaultMeshData Blueprint Hierarchy ---")
    bp = unreal.load_asset("/Game/SoulslikeFramework/Data/PDA_DefaultMeshData")
    if bp and hasattr(bp, 'generated_class'):
        gen_class = bp.generated_class()
        if gen_class:
            log("Hierarchy for PDA_DefaultMeshData_C:")
            print_class_hierarchy(gen_class)

    # Check DA_QuinnMeshDefault mesh properties
    log("\n--- DA_QuinnMeshDefault Mesh Properties ---")
    quinn_mesh = unreal.load_asset("/Game/SoulslikeFramework/Data/BaseCharacters/DA_QuinnMeshDefault")
    if quinn_mesh:
        log(f"Class: {quinn_mesh.get_class().get_name()}")
        for prop in ['head_mesh', 'upper_body_mesh', 'arms_mesh', 'lower_body_mesh']:
            try:
                val = quinn_mesh.get_editor_property(prop)
                if val:
                    # Try to get path from soft object pointer
                    path_str = str(val)
                    log(f"  {prop}: {path_str}")
                else:
                    log(f"  {prop}: (empty/None)")
            except Exception as e:
                log(f"  {prop}: [ERROR] {e}")

    # Check DA_MannyMeshDefault mesh properties
    log("\n--- DA_MannyMeshDefault Mesh Properties ---")
    manny_mesh = unreal.load_asset("/Game/SoulslikeFramework/Data/BaseCharacters/DA_MannyMeshDefault")
    if manny_mesh:
        log(f"Class: {manny_mesh.get_class().get_name()}")
        for prop in ['head_mesh', 'upper_body_mesh', 'arms_mesh', 'lower_body_mesh']:
            try:
                val = manny_mesh.get_editor_property(prop)
                if val:
                    path_str = str(val)
                    log(f"  {prop}: {path_str}")
                else:
                    log(f"  {prop}: (empty/None)")
            except Exception as e:
                log(f"  {prop}: [ERROR] {e}")

    # Check GameInstance SelectedBaseClass default
    log("\n--- GameInstance SelectedBaseClass ---")
    gi = unreal.load_asset("/Game/SoulslikeFramework/Blueprints/Framework/GI_SoulslikeFramework")
    if gi and hasattr(gi, 'generated_class'):
        gen_class = gi.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                try:
                    selected = cdo.get_editor_property('selected_base_class')
                    if selected:
                        name = selected.get_name() if hasattr(selected, 'get_name') else str(selected)
                        log(f"  selected_base_class: {name}")
                    else:
                        log(f"  selected_base_class: (None)")
                except Exception as e:
                    log(f"  selected_base_class: [ERROR] {e}")

    log("\n" + "=" * 80)

if __name__ == "__main__":
    main()
