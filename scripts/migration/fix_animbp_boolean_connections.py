# fix_animbp_boolean_connections.py
# Fix broken boolean variable getter connections in AnimGraph AND GetAccelerationData graph

import unreal

def log(msg):
    print(f"[FixAnimBP] {msg}")
    unreal.log_warning(f"[FixAnimBP] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def main():
    log("=" * 80)
    log("ANIMBP COMPLETE CONNECTION FIX")
    log("=" * 80)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    log(f"Loaded: {bp.get_name()}")

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    total_fixed = 0

    # Step 1: Fix GetAccelerationData graph (CRITICAL - sets bIsAccelerating, bIsFalling)
    log("\nStep 1: Fixing GetAccelerationData graph connections...")
    accel_fixed = unreal.SLFAutomationLibrary.fix_get_acceleration_data_graph(bp)
    log(f"  Fixed {accel_fixed} GetAccelerationData connections")
    total_fixed += accel_fixed

    # Step 2: Fix AnimGraph boolean connections (bIsBlocking, bIsFalling getters)
    log("\nStep 2: Fixing AnimGraph boolean connections...")
    bool_fixed = unreal.SLFAutomationLibrary.fix_anim_graph_boolean_connections(bp)
    log(f"  Fixed {bool_fixed} AnimGraph boolean connections")
    total_fixed += bool_fixed

    # Step 3: Fix BlendListByEnum bindings (overlay states)
    log("\nStep 3: Fixing BlendListByEnum bindings...")
    enum_fixed = unreal.SLFAutomationLibrary.fix_all_blend_list_by_enum_bindings(bp)
    log(f"  Fixed {enum_fixed} BlendListByEnum bindings")
    total_fixed += enum_fixed

    # Step 4: Fix transition graph connections
    log("\nStep 4: Fixing transition graph connections...")
    trans_fixed = unreal.SLFAutomationLibrary.fix_all_broken_transitions(bp)
    log(f"  Fixed {trans_fixed} transition connections")
    total_fixed += trans_fixed

    # Step 5: Compile
    log("\nStep 5: Compiling Blueprint...")
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)

    # Step 6: Save
    log("\nStep 6: Saving asset...")
    unreal.EditorAssetLibrary.save_asset(ANIMBP_PATH)

    log(f"\n[DONE] Total fixed: {total_fixed} connections")
    log(f"  - GetAccelerationData: {accel_fixed}")
    log(f"  - AnimGraph booleans: {bool_fixed}")
    log(f"  - BlendListByEnum: {enum_fixed}")
    log(f"  - Transitions: {trans_fixed}")

if __name__ == "__main__":
    main()
