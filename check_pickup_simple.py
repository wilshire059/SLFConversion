"""
Simple check of B_PickupItem collision
"""
import unreal

unreal.log("=== Starting pickup collision check ===")

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/B_PickupItem"

# Load Blueprint asset
bp_asset = unreal.EditorAssetLibrary.load_asset(bp_path)
if bp_asset:
    unreal.log(f"Loaded: {bp_asset.get_name()}")

    # Get generated class
    gen_class = unreal.EditorAssetLibrary.load_blueprint_class(bp_path)
    if gen_class:
        unreal.log(f"Generated class: {gen_class.get_name()}")

        # Get CDO
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            unreal.log(f"CDO: {cdo.get_name()}")

            # Get SCS from blueprint
            if hasattr(bp_asset, 'simple_construction_script'):
                scs = bp_asset.simple_construction_script
                if scs:
                    nodes = scs.get_all_nodes()
                    unreal.log(f"SCS has {len(nodes)} nodes")
                    for node in nodes:
                        template = node.component_template
                        if template:
                            unreal.log(f"  Node: {template.get_name()} ({template.get_class().get_name()})")
                            if hasattr(template, 'get_collision_enabled'):
                                ce = template.get_collision_enabled()
                                unreal.log(f"    CollisionEnabled: {ce}")
                            if hasattr(template, 'get_collision_object_type'):
                                cot = template.get_collision_object_type()
                                unreal.log(f"    CollisionObjectType: {cot}")
                else:
                    unreal.log("SCS is None")
            else:
                unreal.log("No simple_construction_script attribute")
        else:
            unreal.log("ERROR: Could not get CDO")
    else:
        unreal.log("ERROR: Could not load generated class")
else:
    unreal.log(f"ERROR: Could not load {bp_path}")

unreal.log("=== Done ===")
