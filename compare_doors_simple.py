"""
Simple comparison of B_Door mesh data.
"""
import unreal
import json

results = {"door_data": {}}

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"

unreal.log("=" * 70)
unreal.log("B_Door Mesh Analysis")
unreal.log("=" * 70)

bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if not bp:
    unreal.log_error(f"Could not load {bp_path}")
else:
    results["door_data"]["path"] = bp_path

    # Get parent class
    try:
        parent = bp.get_editor_property('parent_class')
        results["door_data"]["parent_class"] = str(parent.get_name()) if parent else "None"
        unreal.log(f"Parent Class: {results['door_data']['parent_class']}")
    except Exception as e:
        results["door_data"]["parent_class_error"] = str(e)

    gen_class = bp.generated_class()
    if gen_class:
        results["door_data"]["generated_class"] = gen_class.get_name()
        unreal.log(f"Generated Class: {gen_class.get_name()}")

        cdo = unreal.get_default_object(gen_class)
        if cdo:
            results["door_data"]["cdo_components"] = []

            # Get StaticMeshComponents only
            mesh_comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
            unreal.log(f"\nStaticMeshComponents: {len(mesh_comps)}")

            for comp in mesh_comps:
                comp_info = {
                    "name": comp.get_name(),
                    "class": comp.get_class().get_name()
                }
                mesh = comp.static_mesh
                comp_info["mesh_path"] = mesh.get_path_name() if mesh else "None"
                comp_info["mesh_name"] = mesh.get_name() if mesh else "NO_MESH"
                comp_info["visible"] = comp.is_visible()

                results["door_data"]["cdo_components"].append(comp_info)
                unreal.log(f"  [{comp.get_name()}]")
                unreal.log(f"    Mesh: {comp_info['mesh_name']}")
                unreal.log(f"    Visible: {comp_info['visible']}")

            # Check TSoftObjectPtr properties
            results["door_data"]["properties"] = {}
            unreal.log("\nTSoftObjectPtr Properties:")
            try:
                default_mesh = cdo.get_editor_property('default_door_mesh')
                results["door_data"]["properties"]["default_door_mesh"] = str(default_mesh) if default_mesh else "None"
                unreal.log(f"  default_door_mesh: {results['door_data']['properties']['default_door_mesh']}")
            except:
                results["door_data"]["properties"]["default_door_mesh"] = "N/A"
                unreal.log("  default_door_mesh: N/A")

            try:
                default_frame = cdo.get_editor_property('default_door_frame_mesh')
                results["door_data"]["properties"]["default_door_frame_mesh"] = str(default_frame) if default_frame else "None"
                unreal.log(f"  default_door_frame_mesh: {results['door_data']['properties']['default_door_frame_mesh']}")
            except:
                results["door_data"]["properties"]["default_door_frame_mesh"] = "N/A"
                unreal.log("  default_door_frame_mesh: N/A")

    # Get SCS components
    unreal.log("\nSCS Components (Blueprint-owned):")
    results["door_data"]["scs_components"] = []
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            all_nodes = scs.get_all_nodes()
            for node in all_nodes:
                if node:
                    template = node.component_template
                    if template:
                        scs_info = {
                            "name": template.get_name(),
                            "class": template.get_class().get_name()
                        }
                        if isinstance(template, unreal.StaticMeshComponent):
                            mesh = template.static_mesh
                            scs_info["mesh_name"] = mesh.get_name() if mesh else "NO_MESH"
                        results["door_data"]["scs_components"].append(scs_info)
                        unreal.log(f"  [{scs_info['name']}] ({scs_info['class']}): {scs_info.get('mesh_name', 'N/A')}")
        else:
            unreal.log("  No SCS found")
    except Exception as e:
        unreal.log(f"  SCS Error: {e}")

# Save
output_path = "C:/scripts/SLFConversion/migration_cache/door_analysis_slfconv.json"
with open(output_path, "w") as f:
    json.dump(results, f, indent=2)

unreal.log(f"\nSaved to {output_path}")
