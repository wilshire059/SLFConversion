# test_demo_level.py
# Test if the demo level can be loaded

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/demo_level_test_output.txt"

def run():
    results = []
    results.append("=" * 60)
    results.append("DEMO LEVEL LOAD TEST")
    results.append("=" * 60)

    passed = 0
    failed = 0

    def test(name, condition, error=""):
        nonlocal passed, failed
        if condition:
            passed += 1
            results.append(f"[PASS] {name}")
            return True
        else:
            failed += 1
            results.append(f"[FAIL] {name}: {error}")
            return False

    try:
        # 1. Check editor subsystem
        subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
        test("UnrealEditorSubsystem available", subsystem is not None)

        level_editor = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
        test("LevelEditorSubsystem available", level_editor is not None)

        # 2. Check demo level exists
        demo_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
        exists = unreal.EditorAssetLibrary.does_asset_exist(demo_path)
        test("Demo level exists", exists, f"Path: {demo_path}")

        # 3. Load demo level
        if exists and level_editor:
            success = level_editor.load_level(demo_path)
            test("Demo level loaded", success)

            if success and subsystem:
                world = subsystem.get_editor_world()
                test("World available after load", world is not None)
                if world:
                    results.append(f"    World name: {world.get_name()}")

        # 4. Test loading key character blueprints
        results.append("")
        results.append("Testing Key Blueprints:")

        key_bps = [
            "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character",
            "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy",
            "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss",
            "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
        ]

        for bp_path in key_bps:
            bp_name = bp_path.split("/")[-1]
            try:
                bp = unreal.EditorAssetLibrary.load_asset(bp_path)
                if bp:
                    gen_class = bp.generated_class()
                    test(f"{bp_name} loads", gen_class is not None)
                else:
                    test(f"{bp_name} exists", False, "Asset not found")
            except Exception as e:
                test(f"{bp_name} loads", False, str(e))

        # 5. Test loading key component blueprints
        results.append("")
        results.append("Testing Component Blueprints:")

        component_bps = [
            "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager",
            "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager",
            "/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager",
            "/Game/SoulslikeFramework/Blueprints/Components/AC_EquipmentManager",
            "/Game/SoulslikeFramework/Blueprints/Components/AC_InventoryManager",
        ]

        for bp_path in component_bps:
            bp_name = bp_path.split("/")[-1]
            try:
                bp = unreal.EditorAssetLibrary.load_asset(bp_path)
                if bp:
                    gen_class = bp.generated_class()
                    test(f"{bp_name} loads", gen_class is not None)
                else:
                    test(f"{bp_name} exists", False, "Asset not found")
            except Exception as e:
                test(f"{bp_name} loads", False, str(e))

        # 6. Test loading AnimBPs
        results.append("")
        results.append("Testing AnimBPs:")

        anim_bps = [
            "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
            "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
            "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
        ]

        for bp_path in anim_bps:
            bp_name = bp_path.split("/")[-1]
            try:
                bp = unreal.EditorAssetLibrary.load_asset(bp_path)
                if bp:
                    gen_class = bp.generated_class()
                    test(f"{bp_name} loads", gen_class is not None)
                else:
                    test(f"{bp_name} exists", False, "Asset not found")
            except Exception as e:
                test(f"{bp_name} loads", False, str(e))

        # 7. Test loading widgets
        results.append("")
        results.append("Testing Widgets:")

        widgets = [
            "/Game/SoulslikeFramework/Widgets/HUD/W_HUD",
            "/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory",
            "/Game/SoulslikeFramework/Widgets/Equipment/W_Equipment",
        ]

        for bp_path in widgets:
            bp_name = bp_path.split("/")[-1]
            try:
                bp = unreal.EditorAssetLibrary.load_asset(bp_path)
                if bp:
                    gen_class = bp.generated_class()
                    test(f"{bp_name} loads", gen_class is not None)
                else:
                    test(f"{bp_name} exists", False, "Asset not found")
            except Exception as e:
                test(f"{bp_name} loads", False, str(e))

    except Exception as e:
        results.append(f"ERROR: {e}")
        import traceback
        results.append(traceback.format_exc())

    # Summary
    results.append("")
    results.append("=" * 60)
    results.append("SUMMARY")
    results.append("=" * 60)
    results.append(f"Passed: {passed}")
    results.append(f"Failed: {failed}")
    results.append(f"Total:  {passed + failed}")
    results.append("")
    results.append(f"RESULT: {'PASSED' if failed == 0 else 'FAILED'}")
    results.append("=" * 60)

    # Write results
    with open(OUTPUT_FILE, 'w') as f:
        f.write("\n".join(results))

    # Also print
    for line in results:
        print(line)

run()
