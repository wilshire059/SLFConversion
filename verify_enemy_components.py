"""
Verify enemy component hierarchy
Check for duplicate AIBehaviorManagerComponent
"""
import unreal

def log(msg):
    unreal.log_warning(f"[VERIFY_COMP] {msg}")

def check_enemy_components(enemy_path):
    bp = unreal.load_asset(enemy_path)
    if not bp:
        log(f"Could not load: {enemy_path}")
        return

    log(f"")
    log(f"=== {bp.get_name()} ===")

    # Get the CDO
    gen_class = bp.generated_class()
    if not gen_class:
        log(f"  No generated class")
        return

    # Get parent class name
    try:
        parent_name = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"  Parent class: {parent_name}")
    except:
        log(f"  Parent class: (could not determine)")

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        log(f"  No CDO")
        return

    # Find ALL AIBehaviorManagerComponent instances
    all_comps = cdo.get_components_by_class(unreal.ActorComponent)
    log(f"  Total components: {len(all_comps)}")

    bm_count = 0
    for comp in all_comps:
        comp_class = comp.get_class().get_name()
        comp_name = comp.get_name()
        if "Behavior" in comp_class or "BehaviorManager" in comp_name:
            bm_count += 1
            log(f"    [{bm_count}] {comp_name} (class: {comp_class})")

            # Check Behavior property
            try:
                behavior = comp.get_editor_property("behavior")
                log(f"        Behavior: {behavior.get_name() if behavior else 'None'}")
            except Exception as e:
                log(f"        Error getting behavior: {e}")

    if bm_count == 0:
        log(f"  WARNING: No AIBehaviorManagerComponent found!")
    elif bm_count > 1:
        log(f"  WARNING: Multiple ({bm_count}) AIBehaviorManagerComponent found!")
    else:
        log(f"  OK: Exactly 1 AIBehaviorManagerComponent")

    # Also check SCS for Blueprint-added components
    try:
        scs_comps = unreal.SLFAutomationLibrary.get_blueprint_scs_components(bp)
        log(f"  SCS components: {len(scs_comps)}")
        for scs_comp in scs_comps:
            if "Behavior" in str(scs_comp):
                log(f"    SCS: {scs_comp}")
    except:
        pass

def main():
    log("=" * 70)
    log("VERIFYING ENEMY COMPONENT HIERARCHY")
    log("=" * 70)

    enemies = [
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth",
    ]

    for enemy_path in enemies:
        check_enemy_components(enemy_path)

    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
