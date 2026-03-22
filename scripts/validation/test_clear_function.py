# test_clear_function.py
# Simple test to verify ClearAllBlueprintLogicNoCompile works
import unreal

def run():
    print("=" * 60)
    print("TESTING ClearAllBlueprintLogicNoCompile")
    print("=" * 60)

    # Try loading a simple Blueprint
    test_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager"
    print(f"Loading: {test_path}")

    bp = unreal.EditorAssetLibrary.load_asset(test_path)
    if not bp:
        print("ERROR: Could not load Blueprint")
        return

    print(f"Loaded: {bp.get_name()}")

    # Try calling the function
    print("Calling clear_all_blueprint_logic_no_compile...")
    result = unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(bp)
    print(f"Result: {result}")

    print("Test complete!")

run()
