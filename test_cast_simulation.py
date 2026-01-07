# test_cast_simulation.py
# Simulate what Cast<UPDA_Action> would do at runtime

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/check_output.txt"
lines = []

lines.append("=" * 60)
lines.append("SIMULATING Cast<UPDA_Action> FOR ACTION DATA ASSETS")
lines.append("=" * 60)

# Load the C++ class
cpp_class = unreal.load_class(None, "/Script/SLFConversion.PDA_Action")
lines.append(f"\n1. C++ UPDA_Action class:")
lines.append(f"   Path: /Script/SLFConversion.PDA_Action")
lines.append(f"   Loaded: {cpp_class}")

if cpp_class:
    lines.append(f"   Name: {cpp_class.get_name()}")

# Load a data asset
asset_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge"
asset = unreal.EditorAssetLibrary.load_asset(asset_path)

lines.append(f"\n2. DA_Action_Dodge asset:")
lines.append(f"   Loaded: {asset}")

if asset:
    asset_class = asset.get_class()
    lines.append(f"   Class: {asset_class.get_name()}")
    lines.append(f"   Class path: {asset_class.get_path_name()}")

    # Check if asset class is a child of cpp_class
    lines.append(f"\n3. Checking class hierarchy (simulates Cast<UPDA_Action>):")

    if cpp_class:
        # Use is_child_of to check class relationship
        is_child = asset_class.is_child_of(cpp_class)
        lines.append(f"   asset_class.is_child_of(cpp_class): {is_child}")

        if is_child:
            lines.append(f"\n   SUCCESS! Cast<UPDA_Action> would SUCCEED")
            lines.append(f"   Direct access to RelevantData.GetPtr<T>() would work")
        else:
            lines.append(f"\n   FAILURE! Cast<UPDA_Action> would FAIL")
            lines.append(f"   Need to fix reparenting or use reflection")

# Check the Blueprint parent
lines.append(f"\n4. Checking Blueprint PDA_Action:")
bp_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/PDA_Action"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if bp:
    gen_class = bp.generated_class()
    lines.append(f"   Generated class: {gen_class.get_name()}")
    lines.append(f"   Generated class path: {gen_class.get_path_name()}")

    if cpp_class:
        is_bp_child = gen_class.is_child_of(cpp_class)
        lines.append(f"   is_child_of(C++ PDA_Action): {is_bp_child}")

        if is_bp_child:
            lines.append(f"\n   Blueprint PDA_Action IS reparented to C++!")
        else:
            lines.append(f"\n   Blueprint PDA_Action is NOT reparented to C++")

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print(f"Output written to {OUTPUT_FILE}")
