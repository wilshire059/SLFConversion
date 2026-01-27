"""
Apply BehaviorTree to enemy AIBehaviorManagerComponent
The BT assignment was lost during C++ migration
"""
import unreal

def log(msg):
    unreal.log_warning(f"[APPLY_BT] {msg}")

def apply_bt_to_enemy(enemy_path, bt_path):
    """Apply behavior tree to enemy's AIBehaviorManagerComponent"""
    bp = unreal.load_asset(enemy_path)
    if not bp:
        log(f"Could not load: {enemy_path}")
        return False

    bt = unreal.load_asset(bt_path)
    if not bt:
        log(f"Could not load BT: {bt_path}")
        return False

    log(f"Processing: {bp.get_name()}")

    # Get the CDO
    gen_class = bp.generated_class()
    if not gen_class:
        log(f"  No generated class")
        return False

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        log(f"  No CDO")
        return False

    # Find AIBehaviorManagerComponent
    bm_comp = cdo.get_component_by_class(unreal.AIBehaviorManagerComponent)
    if not bm_comp:
        log(f"  No AIBehaviorManagerComponent found")
        return False

    # Check current value
    current_bt = bm_comp.get_editor_property("behavior")
    log(f"  Current BT: {current_bt}")

    # Set the behavior tree
    try:
        bm_comp.set_editor_property("behavior", bt)
        new_bt = bm_comp.get_editor_property("behavior")
        log(f"  Set BT to: {new_bt}")

        # Save the asset
        unreal.EditorAssetLibrary.save_asset(enemy_path)
        log(f"  Saved!")
        return True
    except Exception as e:
        log(f"  Error: {e}")
        return False

def main():
    log("=" * 70)
    log("APPLYING BEHAVIOR TREE TO ENEMIES")
    log("=" * 70)

    # The behavior tree to apply
    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy"

    # All enemy Blueprints that need BT assignment
    enemies = [
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
    ]

    # Boss uses same BT (BT_Boss doesn't exist - they use BT_Enemy)
    bosses = [
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth",
    ]

    success = 0
    failed = 0

    # Apply to regular enemies
    for enemy_path in enemies:
        if apply_bt_to_enemy(enemy_path, bt_path):
            success += 1
        else:
            failed += 1

    # Apply to bosses (same BT as regular enemies)
    for boss_path in bosses:
        if apply_bt_to_enemy(boss_path, bt_path):
            success += 1
        else:
            failed += 1

    log("")
    log("=" * 70)
    log(f"RESULTS: {success} succeeded, {failed} failed")
    log("=" * 70)

if __name__ == "__main__":
    main()
