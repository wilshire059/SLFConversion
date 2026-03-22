# apply_container_meshes.py
# Apply chest box and lid mesh assignments to B_Container Blueprint CDO

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("APPLYING CONTAINER MESHES TO B_Container")
    unreal.log_warning("=" * 70)

    # Load the Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load: {bp_path}")
        return

    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_error("No generated class")
        return

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    unreal.log_warning(f"CDO: {cdo.get_name()}")

    # Load the chest meshes
    # From bp_only spawn: SM_ChestBox and SM_ChestLid
    chest_box_path = "/Game/SoulslikeFramework/Meshes/SM/Chest/SM_ChestBox"
    chest_lid_path = "/Game/SoulslikeFramework/Meshes/SM/Chest/SM_ChestLid"

    chest_box_mesh = unreal.load_asset(chest_box_path)
    chest_lid_mesh = unreal.load_asset(chest_lid_path)

    if not chest_box_mesh:
        unreal.log_error(f"Could not load chest box mesh: {chest_box_path}")
        return
    if not chest_lid_mesh:
        unreal.log_error(f"Could not load chest lid mesh: {chest_lid_path}")
        return

    unreal.log_warning(f"ChestBox mesh: {chest_box_mesh.get_name()}")
    unreal.log_warning(f"ChestLid mesh: {chest_lid_mesh.get_name()}")

    # Set the mesh properties on CDO
    try:
        cdo.set_editor_property("chest_box_mesh", chest_box_mesh)
        unreal.log_warning("Set chest_box_mesh")
    except Exception as e:
        unreal.log_error(f"Failed to set chest_box_mesh: {e}")

    try:
        cdo.set_editor_property("lid_mesh", chest_lid_mesh)
        unreal.log_warning("Set lid_mesh")
    except Exception as e:
        unreal.log_error(f"Failed to set lid_mesh: {e}")

    # Verify
    try:
        box_val = cdo.get_editor_property("chest_box_mesh")
        lid_val = cdo.get_editor_property("lid_mesh")
        unreal.log_warning(f"Verify - chest_box_mesh: {box_val}")
        unreal.log_warning(f"Verify - lid_mesh: {lid_val}")
    except Exception as e:
        unreal.log_warning(f"Verify error: {e}")

    # Save the Blueprint
    unreal.EditorAssetLibrary.save_asset(bp_path)
    unreal.log_warning(f"Saved: {bp_path}")

    unreal.log_warning("=" * 70)
    unreal.log_warning("Done")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
