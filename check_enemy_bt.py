"""
Check what BehaviorTree is assigned to enemy Blueprints
"""
import unreal

def log(msg):
    unreal.log_warning(f"[CHECK_BT] {msg}")

def check_enemy(path):
    bp = unreal.load_asset(path)
    if not bp:
        log(f"Could not load: {path}")
        return

    log(f"")
    log(f"=== {bp.get_name()} ===")

    # Get the CDO
    gen_class = bp.generated_class()
    if not gen_class:
        log(f"  No generated class")
        return

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        log(f"  No CDO")
        return

    # Look for BehaviorManagerComponent
    try:
        # Try to find by name
        bm_comp = cdo.get_component_by_class(unreal.AIBehaviorManagerComponent)
        if bm_comp:
            log(f"  Found AIBehaviorManagerComponent")
            try:
                behavior = bm_comp.get_editor_property("behavior")
                if behavior:
                    log(f"    Behavior: {behavior.get_path_name()}")
                else:
                    log(f"    Behavior: None (NOT SET!)")
            except Exception as e:
                log(f"    Error getting behavior: {e}")
        else:
            log(f"  No AIBehaviorManagerComponent found")

            # List all components
            all_comps = cdo.get_components_by_class(unreal.ActorComponent)
            for comp in all_comps:
                log(f"    - {comp.get_name()} ({comp.get_class().get_name()})")

    except Exception as e:
        log(f"  Error: {e}")

def main():
    log("=" * 70)
    log("CHECKING ENEMY BEHAVIOR TREE ASSIGNMENTS")
    log("=" * 70)

    # Check various enemies with correct paths
    check_enemy("/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy")
    check_enemy("/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard")
    check_enemy("/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth")

    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
