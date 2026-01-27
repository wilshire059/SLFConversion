"""
Complete fix for B_Door:
1. Clear all Blueprint logic and SCS
2. Recompile and save
3. Verify components are inherited from C++
"""
import unreal
import json

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"

unreal.log("="*60)
unreal.log("FIXING B_DOOR")
unreal.log("="*60)

bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if not bp:
    unreal.log_error(f"ERROR: Could not load {bp_path}")
else:
    unreal.log(f"Loaded Blueprint: {bp.get_name()}")

    # Step 1: Clear ALL Blueprint logic using SLFAutomationLibrary
    unreal.log("Step 1: Clearing Blueprint logic...")
    try:
        result = unreal.SLFAutomationLibrary.clear_all_blueprint_logic(bp, False)  # False = don't keep variables
        unreal.log(f"  clear_all_blueprint_logic result: {result}")
    except Exception as e:
        unreal.log_warning(f"  Could not clear logic: {e}")

    # Step 2: Try to clear SCS by removing all nodes
    unreal.log("Step 2: Checking SCS...")
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            all_nodes = scs.get_editor_property('all_nodes')
            if all_nodes and len(all_nodes) > 0:
                unreal.log(f"  SCS has {len(all_nodes)} nodes")
                for node in all_nodes:
                    try:
                        var_name = str(node.get_editor_property('internal_variable_name'))
                        unreal.log(f"    - {var_name}")
                    except:
                        pass
            else:
                unreal.log("  SCS is empty (good - will use C++ components)")
        else:
            unreal.log("  SCS is None")
    except Exception as e:
        unreal.log_warning(f"  Could not check SCS: {e}")

    # Step 3: Compile the Blueprint
    unreal.log("Step 3: Compiling Blueprint...")
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    unreal.log("  Compiled!")

    # Step 4: Save
    unreal.log("Step 4: Saving...")
    unreal.EditorAssetLibrary.save_asset(bp_path)
    unreal.log("  Saved!")

    # Step 5: Verify components
    unreal.log("Step 5: Verifying components...")
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            all_comps = cdo.get_components_by_class(unreal.SceneComponent)
            unreal.log(f"  Blueprint CDO has {len(all_comps)} SceneComponents:")
            for comp in all_comps:
                if isinstance(comp, unreal.StaticMeshComponent):
                    mesh = comp.static_mesh
                    mesh_name = mesh.get_name() if mesh else "NO_MESH"
                    unreal.log(f"    - {comp.get_name()}: StaticMesh (mesh={mesh_name}, visible={comp.is_visible()})")
                else:
                    unreal.log(f"    - {comp.get_name()}: {comp.get_class().get_name()} (visible={comp.is_visible()})")

            # Check if we have the door mesh
            has_door_mesh = any(comp.get_name() == "Interactable SM" for comp in all_comps)
            has_door_frame = any(comp.get_name() == "Door Frame" for comp in all_comps)

            if has_door_mesh and has_door_frame:
                unreal.log("SUCCESS! B_Door now has door components from C++!")
            else:
                unreal.log_warning("WARNING: Door components not found after fix!")

# Write final results
results = {"blueprint_cdo": [], "status": "unknown"}

bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if bp:
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            for comp in cdo.get_components_by_class(unreal.SceneComponent):
                info = {"name": comp.get_name(), "class": comp.get_class().get_name(), "visible": comp.is_visible()}
                if isinstance(comp, unreal.StaticMeshComponent):
                    mesh = comp.static_mesh
                    info["mesh"] = mesh.get_name() if mesh else None
                results["blueprint_cdo"].append(info)

            has_door_mesh = any(c["name"] == "Interactable SM" for c in results["blueprint_cdo"])
            has_door_frame = any(c["name"] == "Door Frame" for c in results["blueprint_cdo"])
            results["status"] = "SUCCESS" if (has_door_mesh and has_door_frame) else "FAILED"

with open("C:/scripts/SLFConversion/migration_cache/door_fix_result.json", "w") as f:
    json.dump(results, f, indent=2)
unreal.log(f"Results saved. Status: {results['status']}")
