# clear_animbp_functions.py
# Clear redundant Blueprint functions from AnimBP while preserving AnimGraph

import unreal
import json
import os

def log(msg):
    unreal.log_warning(f"[CLEAR] {msg}")

# Functions to KEEP (do not clear)
KEEP_FUNCTIONS = {
    "AnimGraph",  # The core animation graph - MUST KEEP
}

# Functions to CLEAR (redundant with C++ NativeUpdateAnimation)
CLEAR_FUNCTIONS = [
    "BlueprintThreadSafeUpdateAnimation",
    "GetLocationData",
    "GetRotationData",
    "GetAccelerationData",
    "GetVelocityData",
    "GetOverlayStates",
    "GetIsBlocking",
    "GetIsResting",
    "GetIsCrouched",
    "GetBlockSequenceForWeapon",
    "GetIkHitReactData",
    "GetCharacterMovementComponent",
    "GetEquipmentComponent",
    "GetCombatComponent",
    "GetActionComponent",
    "GetGrantedTags",
]

def main():
    log("=" * 70)
    log("CLEARING REDUNDANT ANIMBP FUNCTIONS")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    cache_dir = "C:/scripts/SLFConversion/migration_cache"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Step 1: Export state BEFORE clearing
    log("")
    log("=== STEP 1: EXPORT STATE BEFORE CLEARING ===")
    before_file = os.path.join(cache_dir, "animbp_before_clear.json")
    before_state = unreal.SLFAutomationLibrary.export_anim_graph_state(bp, before_file)
    log(f"Exported to: {before_file}")

    # Get current functions
    log("")
    log("=== CURRENT FUNCTIONS ===")
    current_funcs = unreal.SLFAutomationLibrary.get_blueprint_functions(bp)
    log(f"Total functions: {len(current_funcs)}")
    for f in current_funcs:
        status = "KEEP" if f in KEEP_FUNCTIONS else "CLEAR"
        log(f"  [{status}] {f}")

    # Step 2: Clear functions
    log("")
    log("=== STEP 2: CLEARING FUNCTIONS ===")

    cleared_count = 0
    for func_name in CLEAR_FUNCTIONS:
        if func_name in current_funcs:
            success = unreal.SLFAutomationLibrary.remove_function(bp, func_name)
            if success:
                log(f"  [OK] Cleared: {func_name}")
                cleared_count += 1
            else:
                log(f"  [WARN] Failed to clear: {func_name}")
        else:
            log(f"  [--] Not found: {func_name}")

    log(f"Cleared {cleared_count} functions")

    # Compile
    log("")
    log("=== COMPILING ===")
    compile_result = unreal.SLFAutomationLibrary.compile_and_save(bp)
    log(f"Compile result: {compile_result}")

    # Step 3: Export state AFTER clearing
    log("")
    log("=== STEP 3: EXPORT STATE AFTER CLEARING ===")
    after_file = os.path.join(cache_dir, "animbp_after_clear.json")
    after_state = unreal.SLFAutomationLibrary.export_anim_graph_state(bp, after_file)
    log(f"Exported to: {after_file}")

    # Step 4: Validate AnimGraph is intact
    log("")
    log("=== STEP 4: VALIDATING ANIMGRAPH INTACT ===")

    # Parse both states
    try:
        with open(before_file, 'r') as f:
            before_json = json.load(f)
        with open(after_file, 'r') as f:
            after_json = json.load(f)
    except Exception as e:
        log(f"[ERROR] Failed to load JSON: {e}")
        return

    # Compare AnimGraph node counts
    before_animgraph = None
    after_animgraph = None

    for graph in before_json.get("Graphs", []):
        if graph.get("GraphName") == "AnimGraph":
            before_animgraph = graph
            break

    for graph in after_json.get("Graphs", []):
        if graph.get("GraphName") == "AnimGraph":
            after_animgraph = graph
            break

    if before_animgraph and after_animgraph:
        before_nodes = len(before_animgraph.get("Nodes", []))
        after_nodes = len(after_animgraph.get("Nodes", []))

        log(f"AnimGraph nodes BEFORE: {before_nodes}")
        log(f"AnimGraph nodes AFTER: {after_nodes}")

        if before_nodes == after_nodes:
            log("[OK] AnimGraph node count unchanged!")
        else:
            log(f"[WARN] AnimGraph node count changed: {before_nodes} -> {after_nodes}")
    else:
        log("[WARN] Could not find AnimGraph in exports")

    # Check remaining functions
    log("")
    log("=== REMAINING FUNCTIONS ===")
    remaining_funcs = unreal.SLFAutomationLibrary.get_blueprint_functions(bp)
    log(f"Total remaining: {len(remaining_funcs)}")
    for f in remaining_funcs:
        log(f"  {f}")

    # Check compile status
    log("")
    log("=== COMPILE STATUS ===")
    errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
    if errors:
        for line in errors.split('\n')[:30]:
            log(line)
    else:
        log("[OK] No compile errors!")

    # Run diagnostic
    log("")
    log("=== ANIMGRAPH DIAGNOSTIC ===")
    diagnostic = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)

    # Show key sections
    lines = diagnostic.split('\n')
    show_next = 0
    for line in lines:
        if "STATE MACHINE:" in line or "BLEND POSES BY ENUM:" in line or "VARIABLE GET:" in line:
            show_next = 5
        if show_next > 0:
            log(line)
            show_next -= 1

    # Save
    log("")
    log("=== SAVING ===")
    save_result = unreal.EditorAssetLibrary.save_asset(animbp_path, only_if_is_dirty=False)
    log(f"Save result: {save_result}")

    log("")
    log("=== SUMMARY ===")
    log(f"Functions cleared: {cleared_count}")
    log(f"Functions remaining: {len(remaining_funcs)}")
    log(f"AnimGraph intact: {'YES' if before_animgraph and after_animgraph and len(before_animgraph.get('Nodes', [])) == len(after_animgraph.get('Nodes', [])) else 'CHECK'}")

if __name__ == "__main__":
    main()
