#!/usr/bin/env python3
"""Check PC_SoulslikeFramework and cutscene setup"""

import unreal

def log(msg):
    print(f"[Check] {msg}")
    unreal.log_warning(f"[Check] {msg}")

def main():
    log("=" * 70)
    log("CHECKING CUTSCENE SETUP")
    log("=" * 70)

    # 1. Check PC_SoulslikeFramework Blueprint
    log("\n1. PC_SoulslikeFramework Blueprint:")
    pc_path = "/Game/SoulslikeFramework/Blueprints/Global/PC_SoulslikeFramework"
    pc_bp = unreal.EditorAssetLibrary.load_asset(pc_path)

    if pc_bp:
        log(f"   Loaded: {pc_bp.get_name()}")
        if hasattr(unreal, 'SLFAutomationLibrary'):
            parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(pc_bp)
            log(f"   Parent: {parent}")
            errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(pc_bp)
            log(f"   Compile status: {errors[:300]}")
    else:
        log("   ERROR: Could not load Blueprint!")
        # Check if file exists
        if unreal.EditorAssetLibrary.does_asset_exist(pc_path):
            log("   Asset exists but failed to load")
        else:
            log("   Asset does NOT exist!")

    # 2. Check GI_SoulslikeFramework
    log("\n2. GI_SoulslikeFramework:")
    gi_path = "/Game/SoulslikeFramework/Blueprints/Global/GI_SoulslikeFramework"
    gi_bp = unreal.EditorAssetLibrary.load_asset(gi_path)

    if gi_bp:
        log(f"   Loaded: {gi_bp.get_name()}")
        gen_class = gi_bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                try:
                    val = cdo.get_editor_property("first_time_on_demo_level")
                    log(f"   first_time_on_demo_level: {val}")
                except Exception as e:
                    log(f"   Property error: {e}")
    else:
        log("   ERROR: Could not load Blueprint!")

    # 3. Check B_SequenceActor
    log("\n3. B_SequenceActor Blueprint:")
    seq_actor_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor"
    seq_bp = unreal.EditorAssetLibrary.load_asset(seq_actor_path)

    if seq_bp:
        log(f"   Loaded: {seq_bp.get_name()}")
        if hasattr(unreal, 'SLFAutomationLibrary'):
            parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(seq_bp)
            log(f"   Parent: {parent}")
    else:
        log("   ERROR: Could not load Blueprint!")

    # 4. Check Level Sequence
    log("\n4. LS_ShowcaseRoom Level Sequence:")
    ls_path = "/Game/SoulslikeFramework/Cinematics/LS_ShowcaseRoom"
    ls_asset = unreal.EditorAssetLibrary.load_asset(ls_path)

    if ls_asset:
        log(f"   Loaded: {ls_asset.get_name()}")
        log(f"   Class: {ls_asset.get_class().get_name()}")
    else:
        log("   ERROR: Could not load Level Sequence!")

    # 5. Check if World Settings uses correct GameMode
    log("\n5. World Settings (L_Demo_Showcase):")
    # This would require loading the level which is complex in Python
    # The key is that the level uses the correct PlayerController class
    log("   (Verify in editor that GM_Demo uses PC_SoulslikeFramework)")

    log("\n" + "=" * 70)
    log("If PC_SoulslikeFramework has errors, the cutscene spawn logic won't run!")
    log("=" * 70)

if __name__ == "__main__":
    main()
