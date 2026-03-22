# verify_reparent.py
# Verify that PDA_Item is now parented to the C++ UPDA_Item class

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/check_output.txt"

lines = []
lines.append("=" * 60)
lines.append("VERIFYING PDA_Item PARENT CLASS")
lines.append("=" * 60)

try:
    # Load the Blueprint
    bp_path = "/Game/SoulslikeFramework/Data/PDA_Item"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    lines.append(f"\n1. Blueprint: {bp}")

    # Get generated class
    gen_class = bp.generated_class()
    lines.append(f"   Generated class: {gen_class}")
    lines.append(f"   Generated class name: {gen_class.get_name()}")

    # Get parent class using get_super_struct
    if hasattr(gen_class, 'get_super_struct'):
        parent = gen_class.get_super_struct()
        lines.append(f"\n2. Parent class via get_super_struct:")
        lines.append(f"   Parent: {parent}")
        lines.append(f"   Parent name: {parent.get_name() if parent else 'None'}")
        lines.append(f"   Parent path: {parent.get_path_name() if parent else 'None'}")

    # Check the C++ class
    cpp_class = unreal.load_class(None, "/Script/SLFConversion.PDA_Item")
    lines.append(f"\n3. C++ class:")
    lines.append(f"   {cpp_class}")
    lines.append(f"   Name: {cpp_class.get_name() if cpp_class else 'None'}")
    lines.append(f"   Path: {cpp_class.get_path_name() if cpp_class else 'None'}")

    # Check if parent matches C++
    if hasattr(gen_class, 'get_super_struct'):
        parent = gen_class.get_super_struct()
        if parent == cpp_class:
            lines.append(f"\n4. SUCCESS: Parent is C++ class!")
        else:
            lines.append(f"\n4. FAIL: Parent is NOT C++ class")
            lines.append(f"   Expected: {cpp_class}")
            lines.append(f"   Got: {parent}")

    # Load an instance and check for C++ property
    lines.append(f"\n5. Testing C++ properties on DA_Apple:")
    apple = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Data/Items/DA_Apple")
    lines.append(f"   DA_Apple: {apple}")
    lines.append(f"   Class: {apple.get_class()}")
    lines.append(f"   Class name: {apple.get_class().get_name()}")

    # Check for WorldNiagaraSystem property
    if hasattr(apple, 'get_editor_property'):
        try:
            niagara = apple.get_editor_property('WorldNiagaraSystem')
            lines.append(f"   WorldNiagaraSystem: {niagara}")
        except Exception as e:
            lines.append(f"   WorldNiagaraSystem: ERROR - {e}")

    # Check for C++ properties via dir
    props = [p for p in dir(apple) if 'niagara' in p.lower() or 'world' in p.lower()]
    lines.append(f"   Relevant properties: {props}")

except Exception as e:
    import traceback
    lines.append(f"\nERROR: {e}")
    lines.append(f"Traceback: {traceback.format_exc()}")

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print(f"Output written to {OUTPUT_FILE}")
