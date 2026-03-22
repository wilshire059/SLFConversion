# debug_weapon_transform.py
# Debug why weapon transforms aren't persisting

import unreal

def log(msg):
    print(f"[Debug] {msg}")
    unreal.log_warning(f"[Debug] {msg}")

BP_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01"

def main():
    log("=" * 70)
    log("DEBUGGING WEAPON TRANSFORM PERSISTENCE")
    log("=" * 70)

    bp = unreal.load_asset(BP_PATH)
    if not bp:
        log("Could not load Blueprint")
        return

    log(f"Blueprint: {bp.get_name()}")
    log(f"Blueprint class: {type(bp)}")

    # Check generated class
    gen_class = bp.generated_class()
    log(f"Generated class: {gen_class}")

    # Check CDO
    cdo = unreal.get_default_object(gen_class)
    log(f"CDO: {cdo}")

    # Try to get weapon_mesh from CDO
    try:
        mesh_comp = cdo.get_editor_property('weapon_mesh')
        log(f"CDO weapon_mesh: {mesh_comp}")
        if mesh_comp:
            rel_loc = mesh_comp.get_editor_property('relative_location')
            rel_rot = mesh_comp.get_editor_property('relative_rotation')
            log(f"  CDO Location: {rel_loc.x}, {rel_loc.y}, {rel_loc.z}")
            log(f"  CDO Rotation: P={rel_rot.pitch}, Y={rel_rot.yaw}, R={rel_rot.roll}")
    except Exception as e:
        log(f"CDO weapon_mesh error: {e}")

    # Check Blueprint's SimpleConstructionScript (SCS)
    log("\n--- Checking SCS Components ---")
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            log(f"SCS: {scs}")
            # Get all nodes
            all_nodes = scs.get_all_nodes()
            log(f"SCS has {len(all_nodes)} nodes")
            for node in all_nodes:
                comp_template = node.get_editor_property('component_template')
                if comp_template:
                    comp_name = comp_template.get_name()
                    comp_class = comp_template.get_class().get_name()
                    log(f"  Node: {comp_name} ({comp_class})")

                    # Check if this is the static mesh component
                    if 'Mesh' in comp_name or 'Static' in comp_class:
                        try:
                            rel_loc = comp_template.get_editor_property('relative_location')
                            rel_rot = comp_template.get_editor_property('relative_rotation')
                            log(f"    SCS Location: {rel_loc.x}, {rel_loc.y}, {rel_loc.z}")
                            log(f"    SCS Rotation: P={rel_rot.pitch}, Y={rel_rot.yaw}, R={rel_rot.roll}")
                        except Exception as e:
                            log(f"    Could not get transform: {e}")
    except Exception as e:
        log(f"SCS error: {e}")

    # Check if there's a DefaultWeaponMeshTransform property
    log("\n--- Checking C++ Properties ---")
    try:
        # Check for any transform-related properties
        for prop_name in ['default_weapon_mesh_location', 'default_weapon_mesh_rotation',
                          'weapon_mesh_offset', 'mesh_offset', 'default_mesh_transform']:
            try:
                val = cdo.get_editor_property(prop_name)
                log(f"  {prop_name}: {val}")
            except:
                pass
    except Exception as e:
        log(f"Property check error: {e}")

if __name__ == "__main__":
    main()
