"""
Direct Action Manager Test - Test the Blueprint class loading
Writes results to file for headless testing
"""
import unreal
import os

# Output file
OUTPUT_FILE = "C:/scripts/slfconversion/test_results.txt"

def write_log(lines):
    """Write all lines to output file"""
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(lines))

def test_action_lookup():
    """Test the action class lookup mechanism directly"""
    results = []
    results.append("=" * 60)
    results.append("DIRECT ACTION LOOKUP TEST")
    results.append("=" * 60)

    # Test 1: Load action data assets
    results.append("\n--- Test 1: Loading Action Data Assets ---")

    action_mappings = [
        ("Jump", "Jump"),
        ("Dodge", "Dodge"),
        ("StartSprinting", "StartSprinting"),
        ("StopSprinting", "StopSprinting"),
        ("ComboLight_L", "ComboLight_L"),
        ("ComboLight_R", "ComboLight_R"),
        ("ComboHeavy", "ComboHeavy"),
    ]

    loaded_data_assets = 0
    for tag_name, asset_name in action_mappings:
        asset_path = f"/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_{asset_name}.DA_Action_{asset_name}"
        asset = unreal.load_object(None, asset_path)
        if asset:
            results.append(f"  [OK] Loaded: DA_Action_{asset_name}")
            loaded_data_assets += 1
        else:
            results.append(f"  [FAIL] Could not load: {asset_path}")

    # Test 2: Load action Blueprint classes
    results.append("\n--- Test 2: Loading Action Blueprint Classes ---")

    action_class_mappings = [
        ("Jump", "B_Action_Jump"),
        ("Dodge", "B_Action_Dodge"),
        ("StartSprinting", "B_Action_StartSprinting"),
        ("StopSprinting", "B_Action_StopSprinting"),
        ("ComboLight_L", "B_Action_ComboLight_L"),
        ("ComboLight_R", "B_Action_ComboLight_R"),
        ("ComboHeavy", "B_Action_ComboHeavy"),
    ]

    loaded_classes = 0
    for name, class_name in action_class_mappings:
        bp_path = f"/Game/SoulslikeFramework/Data/Actions/ActionLogic/{class_name}.{class_name}"
        bp = unreal.load_object(None, bp_path)
        if bp:
            # Get the generated class from the Blueprint
            if hasattr(bp, 'generated_class'):
                gen_class = bp.generated_class()
                if gen_class:
                    results.append(f"  [OK] {class_name} -> {gen_class.get_name()}")
                    loaded_classes += 1
                else:
                    results.append(f"  [WARN] {class_name} has NULL generated_class")
            else:
                results.append(f"  [WARN] {class_name} loaded but no generated_class attr")
        else:
            results.append(f"  [FAIL] Could not load: {bp_path}")

    # Test 3: Check B_Action base class
    results.append("\n--- Test 3: Checking B_Action Base Blueprint ---")

    b_action_bp_path = "/Game/SoulslikeFramework/Data/Actions/ActionLogic/B_Action.B_Action"
    b_action_bp = unreal.load_object(None, b_action_bp_path)
    if b_action_bp:
        results.append(f"  [OK] Found B_Action Blueprint")
        if hasattr(b_action_bp, 'generated_class'):
            b_action_class = b_action_bp.generated_class()
            if b_action_class:
                results.append(f"  [OK] B_Action generated_class: {b_action_class.get_name()}")
            else:
                results.append(f"  [WARN] B_Action has NULL generated_class")
    else:
        results.append(f"  [FAIL] B_Action Blueprint not found")

    # Test 4: Check UB_Action C++ class
    results.append("\n--- Test 4: Checking UB_Action C++ Base Class ---")

    cpp_class = unreal.find_object(None, "/Script/SLFConversion.UB_Action")
    if cpp_class:
        results.append(f"  [OK] Found UB_Action C++ class via find_object")
    else:
        results.append(f"  [INFO] UB_Action not found via find_object")
        if hasattr(unreal, 'UB_Action'):
            results.append(f"  [OK] UB_Action accessible as unreal.UB_Action")
        else:
            results.append(f"  [INFO] UB_Action not exposed to Python")

    # Test 5: Summary
    results.append("\n" + "=" * 60)
    results.append("SUMMARY")
    results.append("=" * 60)
    results.append(f"  Data Assets loaded: {loaded_data_assets}/{len(action_mappings)}")
    results.append(f"  Blueprint Classes loaded: {loaded_classes}/{len(action_class_mappings)}")

    if loaded_classes == len(action_class_mappings):
        results.append("\n[SUCCESS] All Blueprint classes loaded correctly!")
        results.append("The UClass* change should allow actions to execute.")
    elif loaded_classes > 0:
        results.append(f"\n[PARTIAL] {loaded_classes} classes loaded, some failed")
    else:
        results.append("\n[FAILURE] No Blueprint classes loaded!")

    results.append("\n" + "=" * 60)
    results.append("TEST COMPLETE")
    results.append("=" * 60)

    # Write results to file
    write_log(results)
    unreal.log(f"Test results written to: {OUTPUT_FILE}")

# Run the test
if __name__ == "__main__":
    test_action_lookup()
