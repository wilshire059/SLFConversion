import unreal

def log(msg):
    print(f"[BossParent] {msg}")
    unreal.log_warning(f"[BossParent] {msg}")

log("=" * 70)
log("CHECKING BOSS BLUEPRINT PARENT CLASS")
log("=" * 70)

boss_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss"
boss_bp = unreal.EditorAssetLibrary.load_asset(boss_path)

if boss_bp:
    log(f"Blueprint: {boss_bp.get_name()}")

    # Get parent class
    try:
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(boss_bp)
        log(f"Parent class from automation: {parent}")
    except:
        pass

    gen_class = boss_bp.generated_class()
    if gen_class:
        log(f"Generated class: {gen_class.get_name()}")

        # Walk up the hierarchy
        current = gen_class
        depth = 0
        while current and depth < 10:
            log(f"  [{depth}] {current.get_name()}")
            # Get parent
            try:
                parent = current.get_parent()
                if parent and parent != current:
                    current = parent
                    depth += 1
                else:
                    break
            except:
                break
else:
    log("ERROR: Could not load boss")

# Also check B_Soulslike_Enemy parent
log("\n--- B_Soulslike_Enemy ---")
enemy_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy"
enemy_bp = unreal.EditorAssetLibrary.load_asset(enemy_path)
if enemy_bp:
    try:
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(enemy_bp)
        log(f"Enemy parent class: {parent}")
    except:
        pass

log("=" * 70)
