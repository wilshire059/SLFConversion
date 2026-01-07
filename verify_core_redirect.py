# verify_core_redirect.py
# Verify that PDA_Action is reparented and Core Redirect works for FInstancedStruct

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/check_output.txt"
lines = []

lines.append("=" * 60)
lines.append("VERIFYING PDA_ACTION REPARENT AND CORE REDIRECT")
lines.append("=" * 60)

# Check PDA_Action parent class
bp_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/PDA_Action"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if bp:
    lines.append(f"\n1. PDA_Action Blueprint:")
    lines.append(f"   Asset: {bp}")
    gen_class = bp.generated_class()
    lines.append(f"   Generated class: {gen_class.get_name()}")

    # Check super class chain using get_super_class
    lines.append(f"\n   Parent class chain:")
    current = gen_class.get_super_class() if hasattr(gen_class, 'get_super_class') else None
    depth = 0
    while current and depth < 10:
        parent_name = current.get_name()
        lines.append(f"     {parent_name}")
        if parent_name in ['PDA_Action', 'PrimaryDataAsset', 'Object']:
            break
        current = current.get_super_class() if hasattr(current, 'get_super_class') else None
        depth += 1
else:
    lines.append(f"\n1. ERROR: Could not load PDA_Action")

# Check if C++ UPDA_Action is the parent
lines.append(f"\n2. Checking if C++ UPDA_Action is in chain:")
cpp_class = unreal.load_class(None, "/Script/SLFConversion.PDA_Action")
if cpp_class:
    lines.append(f"   C++ PDA_Action class found: {cpp_class.get_name()}")
else:
    lines.append(f"   C++ PDA_Action class: NOT FOUND")

# Check a DA_Action asset
asset_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge"
asset = unreal.EditorAssetLibrary.load_asset(asset_path)

if asset:
    lines.append(f"\n3. DA_Action_Dodge:")
    lines.append(f"   Class: {asset.get_class().get_name()}")

    # Check if Cast would work (by checking class hierarchy)
    asset_class = asset.get_class()
    is_upda_action = False

    current = asset_class
    depth = 0
    while current and depth < 15:
        class_name = current.get_name()
        if class_name == 'PDA_Action':  # C++ class name without U prefix
            is_upda_action = True
            break
        if class_name == 'Object':
            break
        current = current.get_super_class() if hasattr(current, 'get_super_class') else None
        depth += 1

    lines.append(f"   Has UPDA_Action in hierarchy: {is_upda_action}")

    # Try to get RelevantData via property access
    lines.append(f"\n4. Checking RelevantData:")
    try:
        relevant_data = asset.get_editor_property('RelevantData')
        lines.append(f"   Type: {type(relevant_data)}")

        # Try to get the struct type name
        if relevant_data:
            # Get internal ScriptStruct (this shows the actual loaded type after Core Redirect)
            if hasattr(relevant_data, 'get_script_struct'):
                script_struct = relevant_data.get_script_struct()
                if script_struct:
                    lines.append(f"   ScriptStruct name: {script_struct.get_name()}")
                    lines.append(f"   ScriptStruct path: {script_struct.get_path_name()}")
                else:
                    lines.append(f"   ScriptStruct: NULL (empty FInstancedStruct)")
            else:
                lines.append(f"   get_script_struct not available")
    except Exception as e:
        lines.append(f"   ERROR: {e}")

lines.append("")
lines.append("=" * 60)
lines.append("KEY CHECK:")
lines.append("- If ScriptStruct path is /Script/SLFConversion.SLFDodgeMontages -> Core Redirect WORKS")
lines.append("- If ScriptStruct path is /Game/.../FDodgeMontages -> Core Redirect NOT applied")
lines.append("=" * 60)

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print(f"Output written to {OUTPUT_FILE}")
