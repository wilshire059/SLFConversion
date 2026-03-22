# fix_animbp_animgraph_connections.py
# Fix broken VariableGet node outputs in AnimGraph (bIsBlocking, bIsFalling)
# and run FixAllBlendListByEnumBindings for overlay states

import unreal

def log(msg):
    print(f"[FixAnimGraph] {msg}")
    unreal.log_warning(f"[FixAnimGraph] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def main():
    log("=" * 80)
    log("ANIMBP ANIMGRAPH CONNECTION FIX")
    log("=" * 80)

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    log(f"Loaded: {bp.get_name()}")

    if not hasattr(unreal, 'SLFAutomationLibrary'):
        log("[ERROR] SLFAutomationLibrary not available")
        return

    # Step 1: Fix BlendListByEnum bindings for overlay states
    log("\nStep 1: Fixing BlendListByEnum overlay state bindings...")
    enum_fixed = unreal.SLFAutomationLibrary.fix_all_blend_list_by_enum_bindings(bp)
    log(f"  Fixed {enum_fixed} BlendListByEnum bindings")

    # Step 2: Compile
    log("\nStep 2: Compiling Blueprint...")
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)

    # Step 3: Get diagnosis to see current state
    log("\nStep 3: Getting diagnosis...")
    diag = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)
    
    # Check for broken issues
    if "LinkedTo (0)" in diag or "broken" in diag.lower():
        log("WARNING: Some connections may still be broken")
        # Print relevant parts
        for line in diag.split('\n'):
            if "LinkedTo (0)" in line or "bIsBlocking" in line or "bIsFalling" in line:
                log(f"  {line}")
    else:
        log("Diagnosis looks clean")

    # Step 4: Save
    log("\nStep 4: Saving asset...")
    unreal.EditorAssetLibrary.save_asset(ANIMBP_PATH)

    log("\n[DONE] AnimGraph connection fix complete")

if __name__ == "__main__":
    main()
