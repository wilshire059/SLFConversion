"""
Extract BehaviorTree assignments from backup enemies
Run this on bp_only project!
"""
import unreal
import json

def log(msg):
    unreal.log_warning(f"[EXTRACT_BT] {msg}")

def check_enemy(path):
    bp = unreal.load_asset(path)
    if not bp:
        log(f"Could not load: {path}")
        return None

    log(f"Checking: {bp.get_name()}")

    # Get the CDO
    gen_class = bp.generated_class()
    if not gen_class:
        return None

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        return None

    # Look for AC_AI_BehaviorManager component
    all_comps = cdo.get_components_by_class(unreal.ActorComponent)
    for comp in all_comps:
        comp_class = comp.get_class().get_name()
        if "BehaviorManager" in comp_class or "AC_AI" in comp_class:
            log(f"  Found component: {comp_class}")
            try:
                behavior = comp.get_editor_property("behavior")
                if behavior:
                    bt_path = behavior.get_path_name()
                    log(f"    Behavior: {bt_path}")
                    return bt_path
                else:
                    log(f"    Behavior: None")
            except Exception as e:
                log(f"    Error: {e}")

    return None

def main():
    log("=" * 70)
    log("EXTRACTING BEHAVIOR TREE ASSIGNMENTS FROM BACKUP")
    log("=" * 70)

    results = {}

    enemies = [
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth",
    ]

    for path in enemies:
        bt_path = check_enemy(path)
        if bt_path:
            results[path] = bt_path

    # Save to file
    output_path = "C:/scripts/SLFConversion/migration_cache/enemy_bt_data.json"
    with open(output_path, "w") as f:
        json.dump(results, f, indent=2)

    log(f"")
    log(f"Saved to: {output_path}")
    log(f"Results: {results}")
    log("=" * 70)

if __name__ == "__main__":
    main()
