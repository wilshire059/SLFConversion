# force_reparent_pda_action.py
# Force reparent PDA_Action Blueprint to C++ by working around name collision

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/reparent_result.txt"
lines = []

lines.append("=" * 60)
lines.append("FORCE REPARENTING PDA_ACTION TO C++")
lines.append("=" * 60)

# Load the C++ class
cpp_class = unreal.load_class(None, "/Script/SLFConversion.PDA_Action")
cpp_path = cpp_class.get_path_name() if cpp_class else None
lines.append(f"\n1. C++ class path: {cpp_path}")

# Load the Blueprint
bp_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/PDA_Action"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if not bp:
    lines.append("ERROR: Could not load Blueprint!")
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(lines))
    raise Exception("Cannot load Blueprint")

lines.append(f"\n2. Blueprint: {bp}")
gen_class = bp.generated_class()
lines.append(f"   Generated class: {gen_class.get_name()}")

# Get current parent
current_parent = None
current_parent_path = None
if hasattr(gen_class, 'get_super_class'):
    current_parent = gen_class.get_super_class()
    if current_parent:
        current_parent_path = current_parent.get_path_name()
lines.append(f"   Current parent path: {current_parent_path}")

# Check if already reparented
if current_parent_path == cpp_path:
    lines.append(f"\n3. Already reparented to C++ UPDA_Action!")
else:
    lines.append(f"\n3. Attempting reparent...")
    lines.append(f"   From: {current_parent_path}")
    lines.append(f"   To:   {cpp_path}")

    # Check parent class of the C++ class
    if hasattr(cpp_class, 'get_super_class'):
        cpp_parent = cpp_class.get_super_class()
        if cpp_parent:
            lines.append(f"\n   C++ class parent: {cpp_parent.get_name()} ({cpp_parent.get_path_name()})")

    # Try to use set_editor_property to change parent directly
    lines.append(f"\n4. Trying direct parent change...")
    try:
        # Get Blueprint's ParentClass field using reflection
        bp_class = bp.get_class()
        lines.append(f"   Blueprint asset class: {bp_class.get_name()}")

        # Try setting the parent class property
        # Note: This property might be read-only in some versions
        if hasattr(bp, 'set_editor_property'):
            try:
                bp.set_editor_property('parent_class', cpp_class)
                lines.append("   set_editor_property('parent_class') called")
            except Exception as e:
                lines.append(f"   set_editor_property failed: {e}")

        # Compile and save
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        lines.append("   Blueprint compiled")

        unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
        lines.append("   Blueprint saved")

        # Check if it worked
        gen_class2 = bp.generated_class()
        if hasattr(gen_class2, 'get_super_class'):
            new_parent = gen_class2.get_super_class()
            if new_parent:
                new_parent_path = new_parent.get_path_name()
                lines.append(f"\n5. New parent path: {new_parent_path}")
                if new_parent_path == cpp_path:
                    lines.append("\n   SUCCESS!")
                else:
                    lines.append("\n   Parent did not change")

    except Exception as e:
        lines.append(f"   ERROR: {e}")

# Test with an action asset
lines.append(f"\n" + "=" * 60)
lines.append("TESTING WITH DA_ACTION_DODGE")
lines.append("=" * 60)

action = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge")
if action:
    action_class = action.get_class()
    lines.append(f"\nAsset class: {action_class.get_name()}")

    # Walk parent chain
    lines.append(f"Parent chain:")
    current = action_class
    depth = 0
    found_cpp = False
    while current and depth < 10:
        path = current.get_path_name()
        lines.append(f"  {current.get_name()} -> {path}")
        if "/Script/SLFConversion" in path:
            found_cpp = True
            lines.append("      ^^^ C++ CLASS FOUND!")
        if current.get_name() == 'Object':
            break
        current = current.get_super_class() if hasattr(current, 'get_super_class') else None
        depth += 1

    if found_cpp:
        lines.append(f"\nSUCCESS: Cast<UPDA_Action>() WILL work!")
    else:
        lines.append(f"\nFAILURE: Cast<UPDA_Action>() will FAIL!")

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print(f"Output written to {OUTPUT_FILE}")
