"""
Fix B_Door using SLFAutomationLibrary functions
"""
import unreal
import json

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"
cpp_parent = "/Script/SLFConversion.B_Door"

results = {"steps": [], "final_state": None}

def log_step(msg):
    unreal.log(f"[FIX_B_DOOR] {msg}")
    results["steps"].append(msg)

log_step("Starting B_Door fix process...")

# Step 1: Load Blueprint
bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if not bp:
    log_step("ERROR: Could not load Blueprint")
else:
    log_step(f"Loaded Blueprint: {bp.get_name()}")

    # Step 2: Get current SCS components using automation library
    log_step("Step 2: Checking current SCS components...")
    try:
        scs_components = unreal.SLFAutomationLibrary.get_blueprint_scs_components(bp)
        log_step(f"  SCS components count: {len(scs_components)}")
        for comp in scs_components:
            log_step(f"    - {comp}")
    except Exception as e:
        log_step(f"  get_blueprint_scs_components error: {e}")

    # Step 3: Get current parent class
    log_step("Step 3: Checking parent class...")
    try:
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log_step(f"  Current parent: {parent}")
    except Exception as e:
        log_step(f"  get_blueprint_parent_class error: {e}")

    # Step 4: Clear construction script
    log_step("Step 4: Clearing construction script...")
    try:
        result = unreal.SLFAutomationLibrary.clear_construction_script(bp)
        log_step(f"  clear_construction_script result: {result}")
    except Exception as e:
        log_step(f"  clear_construction_script error: {e}")

    # Step 5: Clear all Blueprint logic
    log_step("Step 5: Clearing all Blueprint logic...")
    try:
        result = unreal.SLFAutomationLibrary.clear_all_blueprint_logic(bp)
        log_step(f"  clear_all_blueprint_logic result: {result}")
    except Exception as e:
        log_step(f"  clear_all_blueprint_logic error: {e}")

    # Step 6: Force reparent to C++ class (even if already parented)
    log_step(f"Step 6: Force reparent to {cpp_parent}...")
    try:
        result = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_parent)
        log_step(f"  reparent_blueprint result: {result}")
    except Exception as e:
        log_step(f"  reparent_blueprint error: {e}")

    # Step 7: Compile and save
    log_step("Step 7: Compile and save...")
    try:
        result = unreal.SLFAutomationLibrary.compile_and_save(bp)
        log_step(f"  compile_and_save result: {result}")
    except Exception as e:
        log_step(f"  compile_and_save error: {e}")

    # Step 8: Check final state
    log_step("Step 8: Checking final state...")

    # Reload to get fresh state
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if bp:
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                all_comps = cdo.get_components_by_class(unreal.SceneComponent)
                log_step(f"  Final BP CDO component count: {len(all_comps)}")
                final_comps = []
                for comp in all_comps:
                    comp_name = comp.get_name()
                    comp_class = comp.get_class().get_name()
                    mesh_name = None
                    if isinstance(comp, unreal.StaticMeshComponent):
                        mesh = comp.static_mesh
                        mesh_name = mesh.get_name() if mesh else "NO_MESH"
                    log_step(f"    - {comp_name}: {comp_class} (mesh={mesh_name})")
                    final_comps.append({"name": comp_name, "class": comp_class, "mesh": mesh_name})

                results["final_state"] = {
                    "component_count": len(all_comps),
                    "components": final_comps,
                    "has_door_mesh": any(c["name"] == "Interactable SM" for c in final_comps),
                    "has_door_frame": any(c["name"] == "Door Frame" for c in final_comps)
                }

                if results["final_state"]["has_door_mesh"] and results["final_state"]["has_door_frame"]:
                    log_step("SUCCESS! B_Door now has door components!")
                else:
                    log_step("FAILED: Door components still missing")

# Save results
with open("C:/scripts/SLFConversion/migration_cache/b_door_fix_result.json", "w") as f:
    json.dump(results, f, indent=2)
log_step("Results saved to migration_cache/b_door_fix_result.json")
