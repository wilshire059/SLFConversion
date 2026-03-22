"""
Fix B_Door by removing conflicting SCS components before reparenting
"""
import unreal
import json

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"
cpp_parent = "/Script/SLFConversion.B_Door"

results = {"steps": [], "final_state": None}

def log_step(msg):
    unreal.log(f"[FIX_B_DOOR] {msg}")
    results["steps"].append(msg)

log_step("Starting B_Door SCS conflict fix...")

# Step 1: Load Blueprint
bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if not bp:
    log_step("ERROR: Could not load Blueprint")
else:
    log_step(f"Loaded Blueprint: {bp.get_name()}")

    # Step 2: Get current SCS components
    log_step("Step 2: Checking current SCS components...")
    try:
        scs_components = unreal.SLFAutomationLibrary.get_blueprint_scs_components(bp)
        log_step(f"  Found {len(scs_components)} SCS components:")
        for comp in scs_components:
            log_step(f"    - {comp}")
    except Exception as e:
        log_step(f"  Error: {e}")

    # Step 3: Remove MoveTo SCS component (conflicts with C++)
    log_step("Step 3: Removing conflicting SCS components...")
    components_to_remove = ["BillboardComponent", "ArrowComponent"]  # MoveTo and DEBUG_OpeningDirection
    for comp_class in components_to_remove:
        try:
            result = unreal.SLFAutomationLibrary.remove_scs_component_by_class(bp, comp_class)
            log_step(f"  remove_scs_component_by_class({comp_class}): {result}")
        except Exception as e:
            log_step(f"  Error removing {comp_class}: {e}")

    # Step 4: Check SCS after removal
    log_step("Step 4: Checking SCS after removal...")
    try:
        scs_components = unreal.SLFAutomationLibrary.get_blueprint_scs_components(bp)
        log_step(f"  Now {len(scs_components)} SCS components:")
        for comp in scs_components:
            log_step(f"    - {comp}")
    except Exception as e:
        log_step(f"  Error: {e}")

    # Step 5: Clear all Blueprint logic
    log_step("Step 5: Clearing all Blueprint logic...")
    try:
        result = unreal.SLFAutomationLibrary.clear_all_blueprint_logic(bp)
        log_step(f"  clear_all_blueprint_logic: {result}")
    except Exception as e:
        log_step(f"  Error: {e}")

    # Step 6: Reparent to C++ class
    log_step(f"Step 6: Reparent to {cpp_parent}...")
    try:
        result = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_parent)
        log_step(f"  reparent_blueprint: {result}")
    except Exception as e:
        log_step(f"  Error: {e}")

    # Step 7: Compile and save
    log_step("Step 7: Compile and save...")
    try:
        result = unreal.SLFAutomationLibrary.compile_and_save(bp)
        log_step(f"  compile_and_save: {result}")
    except Exception as e:
        log_step(f"  Error: {e}")

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
                    visible = comp.is_visible() if hasattr(comp, 'is_visible') else None
                    if isinstance(comp, unreal.StaticMeshComponent):
                        mesh = comp.static_mesh
                        mesh_name = mesh.get_name() if mesh else "NO_MESH"
                    log_step(f"    - {comp_name}: {comp_class} (mesh={mesh_name}, visible={visible})")
                    final_comps.append({"name": comp_name, "class": comp_class, "mesh": mesh_name, "visible": visible})

                results["final_state"] = {
                    "component_count": len(all_comps),
                    "components": final_comps,
                    "has_door_mesh": any(c["name"] == "Interactable SM" for c in final_comps),
                    "has_door_frame": any(c["name"] == "Door Frame" for c in final_comps),
                    "has_moveto": any(c["name"] == "MoveTo" for c in final_comps)
                }

                if results["final_state"]["has_door_mesh"] and results["final_state"]["has_door_frame"]:
                    log_step("SUCCESS! B_Door now has door components from C++!")
                else:
                    log_step("FAILED: Door components still missing")

# Save results
with open("C:/scripts/SLFConversion/migration_cache/b_door_scs_fix_result.json", "w") as f:
    json.dump(results, f, indent=2)
log_step("Results saved to migration_cache/b_door_scs_fix_result.json")
