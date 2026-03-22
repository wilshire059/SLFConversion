"""
Remove duplicate AC_AI_BehaviorManager SCS component from enemies
The C++ native component should be the only one
"""
import unreal

def log(msg):
    unreal.log_warning(f"[REMOVE_DUP] {msg}")

def remove_scs_behavior_manager(bp_path):
    bp = unreal.load_asset(bp_path)
    if not bp:
        log(f"Could not load: {bp_path}")
        return False

    log(f"Processing: {bp.get_name()}")

    # Get SCS components using automation library
    try:
        scs_comps = unreal.SLFAutomationLibrary.get_blueprint_scs_components(bp)
        log(f"  SCS components before: {len(scs_comps)}")

        # Find BehaviorManager SCS component
        for scs_comp in scs_comps:
            comp_str = str(scs_comp)
            if "BehaviorManager" in comp_str or "AC_AI_" in comp_str:
                log(f"  Found SCS BehaviorManager: {comp_str}")

        # Try to remove using the automation library function
        removed = unreal.SLFAutomationLibrary.remove_scs_component_by_class(
            bp,
            "AC_AI_BehaviorManager_C"
        )

        if removed:
            log(f"  REMOVED SCS component!")
            unreal.EditorAssetLibrary.save_asset(bp_path)
            log(f"  Saved!")
            return True
        else:
            log(f"  No SCS BehaviorManager component found to remove (or already removed)")
            return True

    except Exception as e:
        log(f"  Error: {e}")
        return False

def main():
    log("=" * 70)
    log("REMOVING DUPLICATE SCS BEHAVIOR MANAGER COMPONENTS")
    log("=" * 70)

    enemies = [
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth",
    ]

    success = 0
    for bp_path in enemies:
        if remove_scs_behavior_manager(bp_path):
            success += 1

    log("")
    log(f"Processed {success}/{len(enemies)} blueprints")
    log("=" * 70)

if __name__ == "__main__":
    main()
