# check_weapon_blueprint_scs.py
# Check if Blueprint SCS components exist alongside C++ components

import unreal
import json

def log(msg):
    print(f"[SCS CHECK] {msg}")
    unreal.log_warning(f"[SCS CHECK] {msg}")

def main():
    log("=" * 70)
    log("CHECKING WEAPON BLUEPRINT SCS COMPONENTS")
    log("=" * 70)

    weapons = [
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    ]

    for weapon_path in weapons:
        weapon_name = weapon_path.split("/")[-1]
        log(f"")
        log(f"--- {weapon_name} ---")

        bp = unreal.load_asset(weapon_path)
        if not bp:
            log(f"  ERROR: Could not load")
            continue

        # Get the Blueprint's SimpleConstructionScript
        try:
            scs = bp.get_editor_property('simple_construction_script')
            if scs:
                log(f"  Has SCS: Yes")

                # Get all SCS nodes (components defined in Blueprint)
                all_nodes = scs.get_all_nodes()
                log(f"  SCS Node count: {len(all_nodes)}")

                for node in all_nodes:
                    comp_template = node.get_editor_property('component_template')
                    if comp_template:
                        comp_name = comp_template.get_name()
                        comp_class = comp_template.get_class().get_name()
                        log(f"    SCS Node: {comp_name} ({comp_class})")

                        # For StaticMeshComponent, get transform
                        if "StaticMesh" in comp_class:
                            try:
                                rel_loc = comp_template.get_editor_property('relative_location')
                                rel_rot = comp_template.get_editor_property('relative_rotation')
                                log(f"      Relative Location: {rel_loc}")
                                log(f"      Relative Rotation: {rel_rot}")
                            except:
                                pass
            else:
                log(f"  Has SCS: No")
        except Exception as e:
            log(f"  SCS access error: {e}")

        # Also check the generated class default object
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                log(f"  CDO class: {gen_class.get_name()}")

                # Get all components from CDO
                try:
                    comps = cdo.get_components_by_class(unreal.SceneComponent)
                    log(f"  CDO SceneComponents: {len(comps)}")
                    for comp in comps:
                        if comp:
                            log(f"    CDO Component: {comp.get_name()} ({comp.get_class().get_name()})")
                except Exception as e:
                    log(f"  CDO component error: {e}")

    log("")
    log("Done checking weapon Blueprint SCS")

if __name__ == "__main__":
    main()
