"""
Complete fix for B_SkyManager Blueprint after C++ migration.

This script uses the SLFAutomationLibrary C++ functions to:
1. Clear EventGraph (removes stale pin references)
2. Clear function graphs (removes stale function calls)
3. Remove all SCS components (removes duplicate components)
4. Compile and save
5. Validate final state

The C++ SLFSkyManager class provides all components via CreateDefaultSubobject.
"""
import unreal
import os

BP_PATH = "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager"
CPP_CLASS = "/Script/SLFConversion.SLFSkyManager"
OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/skymanager_fix_complete.txt"

def log(msg):
    """Log to both UE and file"""
    unreal.log_warning(msg)
    with open(OUTPUT_FILE, "a", encoding="utf-8") as f:
        f.write(msg + "\n")

def main():
    # Clear previous output
    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write("")

    log("=" * 70)
    log("COMPLETE B_SkyManager MIGRATION FIX (Using SLFAutomationLibrary)")
    log("=" * 70)

    # Load Blueprint
    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)
    if not bp:
        log(f"ERROR: Could not load Blueprint: {BP_PATH}")
        return False

    log(f"\nLoaded Blueprint: {bp.get_name()}")

    # ===================================================================
    # STEP 1: Check current state using SLFAutomationLibrary
    # ===================================================================
    log("\n" + "-" * 50)
    log("STEP 1: Current State Diagnosis")
    log("-" * 50)

    # Get parent class
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Parent class: {parent}")

    # Get SCS components
    scs_comps = unreal.SLFAutomationLibrary.get_blueprint_scs_components(bp)
    log(f"SCS Components ({len(scs_comps)}):")
    for comp in scs_comps:
        log(f"  - {comp}")

    # Get compile errors
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors and "Errors: 0" not in errors:
        log(f"\nCompile Errors:")
        for line in errors.split('\n'):
            if line.strip():
                log(f"  {line}")
    else:
        log("No compile errors (or already clean)")

    # ===================================================================
    # STEP 2: Clear EventGraphs
    # ===================================================================
    log("\n" + "-" * 50)
    log("STEP 2: Clearing EventGraphs")
    log("-" * 50)

    nodes_cleared = unreal.SLFAutomationLibrary.clear_event_graphs(bp)
    log(f"Nodes cleared from EventGraphs: {nodes_cleared}")

    # ===================================================================
    # STEP 3: Clear Function Graphs
    # ===================================================================
    log("\n" + "-" * 50)
    log("STEP 3: Clearing Function Graphs")
    log("-" * 50)

    funcs_cleared = unreal.SLFAutomationLibrary.clear_function_graphs(bp)
    log(f"Nodes cleared from function graphs: {funcs_cleared}")

    # ===================================================================
    # STEP 4: Clear ConstructionScript
    # ===================================================================
    log("\n" + "-" * 50)
    log("STEP 4: Clearing ConstructionScript")
    log("-" * 50)

    result = unreal.SLFAutomationLibrary.clear_construction_script(bp)
    log(f"ClearConstructionScript result: {result}")

    # ===================================================================
    # STEP 5: Remove all Blueprint variables (they come from C++ now)
    # ===================================================================
    log("\n" + "-" * 50)
    log("STEP 5: Removing Blueprint Variables")
    log("-" * 50)

    # First list what we're removing
    variables = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Blueprint variables to remove ({len(variables)}):")
    for var in variables:
        log(f"  - {var}")

    vars_removed = unreal.SLFAutomationLibrary.remove_all_variables(bp)
    log(f"Variables removed: {vars_removed}")

    # ===================================================================
    # STEP 6: Remove ALL SCS Components (duplicates of C++ components)
    # ===================================================================
    log("\n" + "-" * 50)
    log("STEP 6: Removing SCS Components (Blueprint duplicates)")
    log("-" * 50)

    # Get list of SCS components and remove them one by one
    scs_comps = unreal.SLFAutomationLibrary.get_blueprint_scs_components(bp)
    log(f"SCS Components to remove: {len(scs_comps)}")

    removed_count = 0
    max_iterations = 20  # Safety limit

    while len(scs_comps) > 0 and removed_count < max_iterations:
        # Get the first component's class name
        first_comp = scs_comps[0]  # e.g., "DirectionalLight_Sun_GEN_VARIABLE (DirectionalLightComponent)"

        # Extract class name from the format "Name (ClassName)"
        if "(" in first_comp and ")" in first_comp:
            class_name = first_comp.split("(")[1].rstrip(")")
        else:
            class_name = first_comp

        log(f"  Removing component with class: {class_name}")
        result = unreal.SLFAutomationLibrary.remove_scs_component_by_class(bp, class_name)

        if result:
            removed_count += 1
            log(f"    Removed (total: {removed_count})")
        else:
            log(f"    Failed to remove!")
            break

        # Refresh the list
        scs_comps = unreal.SLFAutomationLibrary.get_blueprint_scs_components(bp)

    log(f"Total SCS components removed: {removed_count}")

    # ===================================================================
    # STEP 7: Compile and Save
    # ===================================================================
    log("\n" + "-" * 50)
    log("STEP 7: Compile and Save")
    log("-" * 50)

    result = unreal.SLFAutomationLibrary.compile_and_save(bp)
    log(f"CompileAndSave result: {result}")

    # ===================================================================
    # STEP 8: Verify Final State
    # ===================================================================
    log("\n" + "-" * 50)
    log("STEP 8: Final State Verification")
    log("-" * 50)

    # Reload for fresh state
    bp = unreal.EditorAssetLibrary.load_asset(BP_PATH)

    # Check parent
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Final parent class: {parent}")

    # Check SCS
    scs_final = unreal.SLFAutomationLibrary.get_blueprint_scs_components(bp)
    log(f"Final SCS Components: {len(scs_final)}")
    if len(scs_final) > 0:
        log("WARNING: SCS still has components!")
        for comp in scs_final:
            log(f"  - {comp}")
    else:
        log("SUCCESS: All SCS components removed!")

    # Check variables
    vars_final = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"Final Blueprint variables: {len(vars_final)}")
    if len(vars_final) > 0:
        for var in vars_final:
            log(f"  - {var}")

    # Check compile errors
    errors_final = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors_final and "Errors: 0" not in errors_final:
        log(f"\nFinal Compile Errors:")
        for line in errors_final.split('\n'):
            if line.strip():
                log(f"  {line}")
    else:
        log("No compile errors - Blueprint is clean!")

    # Check CDO components (should come from C++ parent)
    try:
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                components = cdo.get_components_by_class(unreal.ActorComponent)
                log(f"\nCDO Components from C++ parent ({len(components)}):")
                for comp in components:
                    log(f"  - {comp.get_name()} ({comp.get_class().get_name()})")
    except Exception as e:
        log(f"CDO check error: {e}")

    # Summary
    log("\n" + "=" * 70)
    log("SUMMARY")
    log("=" * 70)

    success = True
    if len(scs_final) > 0:
        log("ISSUE: Blueprint still has SCS components (duplicates)")
        success = False
    if errors_final and "Errors: 0" not in errors_final:
        log("ISSUE: Blueprint still has compile errors")
        success = False

    if success:
        log("SUCCESS: Blueprint is clean!")
        log("  - SCS components: 0 (cleared)")
        log("  - Compile errors: 0")
        log("  - Variables: 0 (inherited from C++)")
        log("  - Components come from C++ SLFSkyManager parent class")

    log(f"\nResults written to: {OUTPUT_FILE}")
    return success

if __name__ == "__main__":
    main()
