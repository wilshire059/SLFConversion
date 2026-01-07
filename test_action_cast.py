# test_action_cast.py
# Test if Cast<UPDA_ActionBase> would work by checking class hierarchy

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/cast_test_result.txt"
lines = []

lines.append("=" * 60)
lines.append("TESTING ACTION CAST")
lines.append("=" * 60)

# Load C++ class
cpp_class = unreal.load_class(None, "/Script/SLFConversion.PDA_ActionBase")
lines.append(f"\n1. C++ PDA_ActionBase class:")
if cpp_class:
    lines.append(f"   Found: {cpp_class.get_name()}")
    lines.append(f"   Path: {cpp_class.get_path_name()}")
else:
    lines.append("   NOT FOUND!")

# Load action asset
action = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge")
lines.append(f"\n2. DA_Action_Dodge asset:")
if action:
    lines.append(f"   Loaded: {action.get_name()}")
    action_class = action.get_class()
    lines.append(f"   Class: {action_class.get_name()}")
    lines.append(f"   Class path: {action_class.get_path_name()}")

    # Check if action class derives from C++ class
    if cpp_class:
        # Walk the class hierarchy manually
        lines.append(f"\n3. Class hierarchy check:")
        current = action_class
        found_cpp = False
        depth = 0
        while current and depth < 20:
            class_path = current.get_path_name()
            lines.append(f"   {depth}: {current.get_name()} -> {class_path}")

            if class_path == cpp_class.get_path_name():
                found_cpp = True
                lines.append(f"       *** MATCH! Cast would SUCCEED ***")
                break

            if current.get_name() == 'Object':
                break

            # Try different ways to get parent
            if hasattr(current, 'get_super_struct'):
                parent = current.get_super_struct()
                if parent:
                    current = parent
                else:
                    lines.append(f"       get_super_struct returned None")
                    break
            else:
                lines.append(f"       no get_super_struct method")
                break
            depth += 1

        lines.append(f"\n" + "=" * 60)
        if found_cpp:
            lines.append("RESULT: Cast<UPDA_ActionBase>() WILL SUCCEED!")
        else:
            lines.append("RESULT: Cast<UPDA_ActionBase>() may FAIL")
            lines.append("(Note: Python API may not expose full hierarchy)")
        lines.append("=" * 60)

# Also try to access a property that only exists on UPDA_ActionBase
lines.append(f"\n4. Trying to access UPDA_ActionBase properties:")
try:
    action_class = action.get_editor_property('ActionClass')
    lines.append(f"   ActionClass: {action_class}")
except Exception as e:
    lines.append(f"   ActionClass error: {e}")

try:
    relevant_data = action.get_editor_property('RelevantData')
    lines.append(f"   RelevantData: {relevant_data}")
    if relevant_data and hasattr(relevant_data, 'get_script_struct'):
        ss = relevant_data.get_script_struct()
        if ss:
            lines.append(f"   RelevantData struct: {ss.get_name()}")
except Exception as e:
    lines.append(f"   RelevantData error: {e}")

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print(f"Output written to {OUTPUT_FILE}")
