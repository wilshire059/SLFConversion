"""Check B_BossDoor SCS node details in bp_only"""
import unreal
import json

BOSS_DOOR_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"
OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/boss_door_scs_bponly.json"

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("CHECKING B_BossDoor SCS")
    unreal.log_warning("=" * 60)

    bp = unreal.load_asset(BOSS_DOOR_PATH)
    if not bp:
        unreal.log_error(f"Could not load {BOSS_DOOR_PATH}")
        return

    unreal.log_warning(f"Blueprint: {bp.get_name()}")

    # Access SimpleConstructionScript
    scs = bp.get_editor_property('simple_construction_script')
    if not scs:
        unreal.log_warning("No SCS found - checking parent")
        # Try parent class B_Door
        parent_bp = unreal.load_asset("/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door")
        if parent_bp:
            scs = parent_bp.get_editor_property('simple_construction_script')

    if scs:
        unreal.log_warning(f"SCS: {scs}")
        nodes = scs.get_editor_property('all_nodes')
        unreal.log_warning(f"SCS Nodes: {len(nodes)}")

        result = {"nodes": []}

        for node in nodes:
            var_name = node.get_editor_property('internal_variable_name')
            unreal.log_warning(f"  Node: {var_name}")

            # Get component template
            template = node.get_editor_property('component_template')
            if template:
                node_data = {
                    "name": str(var_name),
                    "class": template.get_class().get_name()
                }

                # Get transform if it's a scene component
                try:
                    rel_loc = template.get_editor_property('relative_location')
                    rel_rot = template.get_editor_property('relative_rotation')
                    rel_scale = template.get_editor_property('relative_scale3d')
                    node_data["relative_location"] = {"x": rel_loc.x, "y": rel_loc.y, "z": rel_loc.z}
                    node_data["relative_rotation"] = {"pitch": rel_rot.pitch, "yaw": rel_rot.yaw, "roll": rel_rot.roll}
                    node_data["relative_scale3d"] = {"x": rel_scale.x, "y": rel_scale.y, "z": rel_scale.z}
                    unreal.log_warning(f"    Class: {template.get_class().get_name()}")
                    unreal.log_warning(f"    Location: ({rel_loc.x:.2f}, {rel_loc.y:.2f}, {rel_loc.z:.2f})")
                    unreal.log_warning(f"    Rotation: ({rel_rot.pitch:.1f}, {rel_rot.yaw:.1f}, {rel_rot.roll:.1f})")
                    unreal.log_warning(f"    Scale: ({rel_scale.x:.4f}, {rel_scale.y:.4f}, {rel_scale.z:.4f})")
                except:
                    pass

                # Get static mesh if it's a StaticMeshComponent
                try:
                    mesh = template.get_editor_property('static_mesh')
                    if mesh:
                        node_data["static_mesh"] = mesh.get_name()
                        unreal.log_warning(f"    Mesh: {mesh.get_name()}")
                except:
                    pass

                result["nodes"].append(node_data)

        # Save to JSON
        with open(OUTPUT_PATH, 'w') as f:
            json.dump(result, f, indent=2)
        unreal.log_warning(f"Saved to {OUTPUT_PATH}")

    else:
        unreal.log_error("Could not find SCS")

    unreal.log_warning("=" * 60)

if __name__ == "__main__":
    main()
