"""
Extract BehaviorTree assignments from backup enemies - v2
Enumerate all component properties
Run this on bp_only project!
"""
import unreal

def log(msg):
    unreal.log_warning(f"[EXTRACT_BT] {msg}")

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

    # List ALL components
    all_comps = cdo.get_components_by_class(unreal.ActorComponent)
    log(f"  Components ({len(all_comps)}):")
    for comp in all_comps:
        comp_name = comp.get_name()
        comp_class = comp.get_class().get_name()
        log(f"    - {comp_name} ({comp_class})")

        # If it's an AI-related component, check all properties
        if "AI" in comp_class or "Behavior" in comp_class:
            log(f"      Checking properties...")
            try:
                # Get class properties
                prop_list = dir(comp)
                for prop in prop_list:
                    if not prop.startswith("_") and "behavior" in prop.lower():
                        try:
                            val = getattr(comp, prop)
                            log(f"        {prop} = {val}")
                        except:
                            pass
            except Exception as e:
                log(f"      Error: {e}")

def main():
    log("=" * 70)
    log("EXTRACTING ENEMY COMPONENT INFO FROM BACKUP")
    log("=" * 70)

    check_enemy("/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy")

    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
