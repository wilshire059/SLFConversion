"""
Remove ALL SCS components from B_Door to fully inherit from C++
"""
import unreal
import json

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"
cpp_parent = "/Script/SLFConversion.B_Door"

results = {"steps": [], "final_state": None}

def log_step(msg):
    unreal.log(f"[FIX_B_DOOR] {msg}")
    results["steps"].append(msg)

log_step("Starting B_Door - remove ALL SCS components...")

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

    # Step 3: Remove ALL SCS components by type
    log_step("Step 3: Removing ALL SCS components...")
    components_to_remove = ["StaticMeshComponent", "BillboardComponent", "ArrowComponent", "SceneComponent"]
    for comp_class in components_to_remove:
        try:
            # Try multiple times in case there are multiple of same type
            for _ in range(5):
                result = unreal.SLFAutomationLibrary.remove_scs_component_by_class(bp, comp_class)
                if not result:
                    break
                log_step(f"  Removed {comp_class}")
        except Exception as e:
            log_step(f"  Error removing {comp_class}: {e}")

    # Step 4: Check SCS after removal
    log_step("Step 4: Checking SCS after removal...")
    try:
        scs_components = unreal.SLFAutomationLibrary.get_blueprint_scs_components(bp)
        log_step(f"  Now {len(scs_components)} SCS components")
        for comp in scs_components:
            log_step(f"    - {comp}")
    except Exception as e:
        log_step(f"  Error: {e}")

    # Step 5: Reparent to C++ class (force refresh)
    log_step(f"Step 5: Reparent to {cpp_parent}...")
    try:
        result = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_parent)
        log_step(f"  reparent_blueprint: {result}")
    except Exception as e:
        log_step(f"  Error: {e}")

    # Step 6: Compile and save
    log_step("Step 6: Compile and save...")
    try:
        result = unreal.SLFAutomationLibrary.compile_and_save(bp)
        log_step(f"  compile_and_save: {result}")
    except Exception as e:
        log_step(f"  Error: {e}")

    # Step 7: Check final state
    log_step("Step 7: Checking final state...")

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

                # Check for door mesh (should now have SM_PrisonDoor from C++)
                door_mesh_found = None
                for c in final_comps:
                    if c["name"] == "Interactable SM":
                        door_mesh_found = c.get("mesh")
                        break

                results["final_state"] = {
                    "component_count": len(all_comps),
                    "components": final_comps,
                    "door_mesh": door_mesh_found,
                    "has_door_mesh": door_mesh_found is not None and door_mesh_found != "NO_MESH",
                    "has_door_frame": any(c["name"] == "Door Frame" for c in final_comps),
                    "has_moveto": any(c["name"] == "MoveTo" for c in final_comps)
                }

                if results["final_state"]["has_door_mesh"]:
                    log_step(f"SUCCESS! Door mesh: {door_mesh_found}")
                else:
                    log_step(f"PARTIAL: Door mesh still missing (got: {door_mesh_found})")

# Save results
with open("C:/scripts/SLFConversion/migration_cache/b_door_final_fix.json", "w") as f:
    json.dump(results, f, indent=2)
log_step("Results saved to migration_cache/b_door_final_fix.json")
