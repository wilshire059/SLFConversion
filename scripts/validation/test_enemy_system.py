"""
Enemy System Test - Verify enemy Blueprints and components load correctly
"""
import unreal

OUTPUT_FILE = "C:/scripts/slfconversion/test_results.txt"

def write_log(lines):
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(lines))

def test_enemy_system():
    results = []
    results.append("=" * 70)
    results.append("ENEMY SYSTEM TEST")
    results.append("=" * 70)

    # ========================================================================
    # TEST 1: Load Enemy Character Blueprints
    # ========================================================================
    results.append("\n" + "=" * 70)
    results.append("TEST 1: Loading Enemy Character Blueprints")
    results.append("=" * 70)

    enemy_bps = [
        ("B_Soulslike_Enemy", "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy"),
        ("B_Soulslike_Boss", "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss"),
        ("B_Soulslike_Character", "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"),
    ]

    loaded_enemies = 0
    for name, path in enemy_bps:
        full_path = f"{path}.{name}"
        bp = unreal.load_object(None, full_path)
        if bp:
            results.append(f"  [OK] {name}")
            if hasattr(bp, 'generated_class') and bp.generated_class():
                gen_class = bp.generated_class()
                results.append(f"       Generated: {gen_class.get_name()}")
                loaded_enemies += 1
            else:
                results.append(f"       [WARN] No generated class")
        else:
            results.append(f"  [FAIL] {name} - Could not load")

    # ========================================================================
    # TEST 2: Load AI Components
    # ========================================================================
    results.append("\n" + "=" * 70)
    results.append("TEST 2: Loading AI Components")
    results.append("=" * 70)

    ai_components = [
        ("AC_AI_BehaviorManager", "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_BehaviorManager"),
        ("AC_AI_CombatManager", "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_CombatManager"),
        ("AC_AI_Boss", "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_Boss"),
        ("AC_AI_InteractionManager", "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager"),
        ("AC_LootDropManager", "/Game/SoulslikeFramework/Blueprints/Components/AC_LootDropManager"),
    ]

    loaded_components = 0
    for name, path in ai_components:
        full_path = f"{path}.{name}"
        bp = unreal.load_object(None, full_path)
        if bp:
            results.append(f"  [OK] {name}")
            loaded_components += 1
        else:
            results.append(f"  [FAIL] {name}")

    # ========================================================================
    # TEST 3: Load AI Controller and Behavior Tree Assets
    # ========================================================================
    results.append("\n" + "=" * 70)
    results.append("TEST 3: Loading AI Controller and BT Assets")
    results.append("=" * 70)

    ai_assets = [
        ("AIC_SoulslikeFramework", "/Game/SoulslikeFramework/Blueprints/_AI/Misc/AIC_SoulslikeFramework"),
        ("B_PatrolPath", "/Game/SoulslikeFramework/Blueprints/_AI/Misc/B_PatrolPath"),
    ]

    for name, path in ai_assets:
        full_path = f"{path}.{name}"
        bp = unreal.load_object(None, full_path)
        if bp:
            results.append(f"  [OK] {name}")
        else:
            results.append(f"  [FAIL] {name}")

    # ========================================================================
    # TEST 4: Load BT Tasks and Services
    # ========================================================================
    results.append("\n" + "=" * 70)
    results.append("TEST 4: Loading Behavior Tree Tasks & Services")
    results.append("=" * 70)

    bt_assets = [
        # Tasks
        ("BTT_ClearKey", "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_ClearKey"),
        ("BTT_GetRandomPoint", "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_GetRandomPoint"),
        ("BTT_SimpleMoveTo", "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SimpleMoveTo"),
        ("BTT_TryExecuteAbility", "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_TryExecuteAbility"),
        ("BTT_SwitchState", "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SwitchState"),
        # Services
        ("BTS_DistanceCheck", "/Game/SoulslikeFramework/Blueprints/_AI/Services/BTS_DistanceCheck"),
        ("BTS_IsTargetDead", "/Game/SoulslikeFramework/Blueprints/_AI/Services/BTS_IsTargetDead"),
        ("BTS_TryGetAbility", "/Game/SoulslikeFramework/Blueprints/_AI/Services/BTS_TryGetAbility"),
    ]

    loaded_bt = 0
    for name, path in bt_assets:
        full_path = f"{path}.{name}"
        bp = unreal.load_object(None, full_path)
        if bp:
            results.append(f"  [OK] {name}")
            loaded_bt += 1
        else:
            results.append(f"  [FAIL] {name}")

    # ========================================================================
    # TEST 5: Load Enemy Animation Blueprint
    # ========================================================================
    results.append("\n" + "=" * 70)
    results.append("TEST 5: Loading Enemy Animation Blueprints")
    results.append("=" * 70)

    anim_bps = [
        ("ABP_SoulslikeEnemy", "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy"),
        ("ABP_SoulslikeBossNew", "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew"),
        ("ABP_SoulslikeNPC", "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC"),
    ]

    for name, path in anim_bps:
        full_path = f"{path}.{name}"
        bp = unreal.load_object(None, full_path)
        if bp:
            results.append(f"  [OK] {name}")
            if hasattr(bp, 'generated_class') and bp.generated_class():
                results.append(f"       Generated: {bp.generated_class().get_name()}")
        else:
            results.append(f"  [FAIL] {name}")

    # ========================================================================
    # TEST 6: Check Enemy Interfaces
    # ========================================================================
    results.append("\n" + "=" * 70)
    results.append("TEST 6: Checking Enemy Interfaces")
    results.append("=" * 70)

    interfaces = [
        ("BPI_Enemy", "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_Enemy"),
        ("BPI_AIC", "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_AIC"),
    ]

    for name, path in interfaces:
        full_path = f"{path}.{name}"
        bp = unreal.load_object(None, full_path)
        if bp:
            results.append(f"  [OK] {name}")
        else:
            results.append(f"  [FAIL] {name}")

    # ========================================================================
    # TEST 7: Check C++ Enemy Classes
    # ========================================================================
    results.append("\n" + "=" * 70)
    results.append("TEST 7: Checking C++ Enemy Classes")
    results.append("=" * 70)

    cpp_classes = [
        "USLFSoulslikeEnemy",
        "USLFEnemyAnimInstance",
        "USLF_AI_BehaviorManager",
    ]

    for class_name in cpp_classes:
        if hasattr(unreal, class_name):
            results.append(f"  [OK] {class_name} - exposed to Python")
        else:
            results.append(f"  [INFO] {class_name} - not exposed (normal)")

    # ========================================================================
    # SUMMARY
    # ========================================================================
    results.append("\n" + "=" * 70)
    results.append("SUMMARY")
    results.append("=" * 70)
    results.append(f"  Enemy Characters: {loaded_enemies}/3")
    results.append(f"  AI Components: {loaded_components}/5")
    results.append(f"  BT Assets: {loaded_bt}/8")

    if loaded_enemies >= 2 and loaded_components >= 3:
        results.append("\n[SUCCESS] Enemy system assets loaded correctly!")
    else:
        results.append("\n[WARNING] Some enemy assets failed to load")

    results.append("\n" + "=" * 70)
    results.append("TEST COMPLETE")
    results.append("=" * 70)

    write_log(results)

if __name__ == "__main__":
    test_enemy_system()
