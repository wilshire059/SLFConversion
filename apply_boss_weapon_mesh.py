"""
Apply boss weapon mesh to B_Item_AI_Weapon_BossMace.
Run on SLFConversion project.
"""
import unreal

BOSS_WEAPON_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_BossMace"
BOSS_WEAPON_MESH_PATH = "/Game/SoulslikeFramework/Demo/ParagonGrux/Characters/Heroes/Grux/Meshes/SM_Grux_Weapon"

def apply_boss_weapon_mesh():
    # Load the boss weapon blueprint
    bp = unreal.EditorAssetLibrary.load_asset(BOSS_WEAPON_PATH)
    if not bp:
        unreal.log_warning(f"Could not load: {BOSS_WEAPON_PATH}")
        return

    # Load the mesh
    mesh = unreal.EditorAssetLibrary.load_asset(BOSS_WEAPON_MESH_PATH)
    if not mesh:
        unreal.log_warning(f"Could not load mesh: {BOSS_WEAPON_MESH_PATH}")
        return

    # Get the generated class and CDO
    gen_class = bp.generated_class()
    if not gen_class:
        unreal.log_warning("No generated class")
        return

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log_warning("No CDO")
        return

    # Try setting DefaultWeaponMesh property (if it exists on C++ parent)
    try:
        cdo.set_editor_property("default_weapon_mesh", mesh)
        unreal.log_warning(f"Set DefaultWeaponMesh to: {BOSS_WEAPON_MESH_PATH}")
    except Exception as e:
        unreal.log_warning(f"DefaultWeaponMesh property error: {e}")

    # Also try to find and set the WeaponMesh component's static mesh
    try:
        mesh_comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
        for comp in mesh_comps:
            comp_name = comp.get_name()
            unreal.log_warning(f"Found StaticMeshComponent: {comp_name}")
            if "weapon" in comp_name.lower() or "mesh" in comp_name.lower():
                comp.set_editor_property("static_mesh", mesh)
                unreal.log_warning(f"  Set static_mesh on {comp_name}")
    except Exception as e:
        unreal.log_warning(f"Component mesh setting error: {e}")

    # Save the blueprint
    unreal.EditorAssetLibrary.save_asset(BOSS_WEAPON_PATH)
    unreal.log_warning(f"Saved: {BOSS_WEAPON_PATH}")

if __name__ == "__main__":
    apply_boss_weapon_mesh()
