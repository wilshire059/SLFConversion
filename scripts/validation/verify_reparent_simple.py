# verify_reparent_simple.py
# Check if PDA_Action has UPDA_Action in its parent chain

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/reparent_result.txt"
lines = []

# Load C++ class
cpp_class = unreal.load_class(None, "/Script/SLFConversion.PDA_Action")
lines.append(f"C++ PDA_Action class: {cpp_class}")
if cpp_class:
    lines.append(f"  Name: {cpp_class.get_name()}")
    lines.append(f"  Path: {cpp_class.get_path_name()}")

# Load the Blueprint PDA_Action
bp_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/PDA_Action"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if bp:
    lines.append(f"\nBlueprint PDA_Action: {bp}")
    gen_class = bp.generated_class()
    lines.append(f"  Generated class: {gen_class.get_name()}")

    # Walk the parent chain
    lines.append(f"\nParent class chain:")
    current = gen_class
    depth = 0
    found_cpp = False
    while current and depth < 15:
        class_name = current.get_name()
        class_path = current.get_path_name()
        lines.append(f"  {depth}: {class_name} -> {class_path}")

        # Check if this is the C++ class
        if "/Script/SLFConversion.PDA_Action" in class_path:
            found_cpp = True
            lines.append(f"      ^^^ THIS IS THE C++ UPDA_Action CLASS!")

        if class_name == 'Object' or class_name == 'PrimaryDataAsset':
            break

        # Get super class
        if hasattr(current, 'get_super_class'):
            current = current.get_super_class()
        else:
            break
        depth += 1

    lines.append(f"\n" + "=" * 50)
    if found_cpp:
        lines.append("SUCCESS: PDA_Action IS reparented to C++ UPDA_Action!")
        lines.append("Cast<UPDA_Action>() WILL work!")
    else:
        lines.append("FAILURE: PDA_Action is NOT reparented to C++ UPDA_Action!")
        lines.append("Cast<UPDA_Action>() will FAIL!")
    lines.append("=" * 50)

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print("Output written to " + OUTPUT_FILE)
