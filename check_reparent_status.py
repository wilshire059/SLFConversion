# check_reparent_status.py
# Check if PDA_Action was properly reparented to UPDA_Action

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/check_output.txt"
lines = []

lines.append("=" * 60)
lines.append("CHECKING PDA_ACTION REPARENT STATUS")
lines.append("=" * 60)

# Load Blueprint
bp_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/PDA_Action"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if bp:
    lines.append(f"\n1. Blueprint asset: {bp}")

    # Check parent_class property on Blueprint
    if hasattr(bp, 'get_editor_property'):
        try:
            parent_class = bp.get_editor_property('ParentClass')
            lines.append(f"   ParentClass property: {parent_class}")
            if parent_class:
                lines.append(f"   Parent class name: {parent_class.get_name()}")
                lines.append(f"   Parent class path: {parent_class.get_path_name()}")
        except Exception as e:
            lines.append(f"   ParentClass error: {e}")

    # Check generated class super using get_super_struct (works for classes)
    gen_class = bp.generated_class()
    if gen_class:
        lines.append(f"\n2. Generated class: {gen_class.get_name()}")
        if hasattr(gen_class, 'get_super_struct'):
            super_class = gen_class.get_super_struct()
            if super_class:
                lines.append(f"   Super (via get_super_struct): {super_class.get_name()}")
                lines.append(f"   Super path: {super_class.get_path_name()}")

                # Check if it's the C++ class
                if "SLFConversion" in super_class.get_path_name():
                    lines.append(f"\n   SUCCESS: Reparented to C++ class!")
                else:
                    lines.append(f"\n   WARNING: Still using Blueprint/other parent")

# Also check the C++ class directly
lines.append(f"\n3. C++ UPDA_Action class:")
cpp_class = unreal.load_class(None, "/Script/SLFConversion.PDA_Action")
if cpp_class:
    lines.append(f"   Found: {cpp_class}")
    lines.append(f"   Name: {cpp_class.get_name()}")
    if hasattr(cpp_class, 'get_super_struct'):
        cpp_super = cpp_class.get_super_struct()
        if cpp_super:
            lines.append(f"   Super: {cpp_super.get_name()}")

# Check a child data asset
lines.append(f"\n4. Child asset DA_Action_Dodge:")
child = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge")
if child:
    child_class = child.get_class()
    lines.append(f"   Class: {child_class.get_name()}")
    if hasattr(child_class, 'get_super_struct'):
        child_super = child_class.get_super_struct()
        depth = 0
        while child_super and depth < 10:
            lines.append(f"   -> {child_super.get_name()} ({child_super.get_path_name()})")
            if child_super.get_name() == 'Object':
                break
            child_super = child_super.get_super_struct() if hasattr(child_super, 'get_super_struct') else None
            depth += 1

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print(f"Output written to {OUTPUT_FILE}")
