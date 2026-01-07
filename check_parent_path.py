# check_parent_path.py
# Check PDA_Action Blueprint parent class by PATH not name

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/reparent_result.txt"
lines = []

lines.append("=" * 60)
lines.append("CHECKING PDA_ACTION PARENT CLASS BY PATH")
lines.append("=" * 60)

# Load the Blueprint
bp_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/PDA_Action"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if not bp:
    lines.append("ERROR: Could not load Blueprint!")
else:
    lines.append(f"\nBlueprint: {bp}")
    lines.append(f"Blueprint Name: {bp.get_name()}")

    # Check Blueprint's ParentClass directly using export_text
    try:
        export = unreal.EditorAssetLibrary.get_metadata_tag(bp, "ParentClass")
        lines.append(f"\nParentClass metadata tag: {export}")
    except:
        lines.append(f"\nParentClass metadata: not available")

    # Get the generated class and walk up
    gen_class = bp.generated_class()
    lines.append(f"\nGenerated class: {gen_class.get_name()}")
    lines.append(f"Generated class path: {gen_class.get_path_name()}")

    # Try to get parent using struct API (works for UClass)
    if hasattr(gen_class, 'get_super_struct'):
        super_struct = gen_class.get_super_struct()
        if super_struct:
            lines.append(f"\nSuper (via get_super_struct):")
            lines.append(f"  Name: {super_struct.get_name()}")
            lines.append(f"  Path: {super_struct.get_path_name()}")

            if "/Script/SLFConversion" in super_struct.get_path_name():
                lines.append("\n  *** C++ PARENT DETECTED! ***")
            elif "/Game/" in super_struct.get_path_name():
                lines.append("\n  Blueprint parent (not C++)")
            else:
                lines.append("\n  Engine parent")

# Test with a data asset
lines.append("\n" + "=" * 60)
lines.append("TESTING DA_ACTION_DODGE HIERARCHY")
lines.append("=" * 60)

action = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge")
if action:
    action_class = action.get_class()
    lines.append(f"\nAsset class: {action_class.get_name()}")
    lines.append(f"Asset class path: {action_class.get_path_name()}")

    # Walk parent chain using get_super_struct
    lines.append("\nParent chain (via get_super_struct):")
    current = action_class
    depth = 0
    found_cpp = False
    while current and depth < 15:
        path = current.get_path_name()
        lines.append(f"  {depth}: {current.get_name()} -> {path}")
        if "/Script/SLFConversion" in path:
            found_cpp = True
            lines.append("       ^^^ C++ CLASS!")
        if current.get_name() == 'Object':
            break
        if hasattr(current, 'get_super_struct'):
            current = current.get_super_struct()
        else:
            lines.append("       (no get_super_struct)")
            break
        depth += 1

    lines.append("\n" + "=" * 60)
    if found_cpp:
        lines.append("RESULT: Cast<UPDA_Action>() WILL work!")
    else:
        lines.append("RESULT: Cast<UPDA_Action>() will FAIL - no C++ in hierarchy")
    lines.append("=" * 60)

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print(f"Output written to {OUTPUT_FILE}")
