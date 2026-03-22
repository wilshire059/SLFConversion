# do_reparent_pda_action.py
# Actually reparent PDA_Action Blueprint to C++ UPDA_Action

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/reparent_result.txt"
lines = []

lines.append("=" * 60)
lines.append("REPARENTING PDA_ACTION TO C++ UPDA_ACTION")
lines.append("=" * 60)

# Load the C++ class
cpp_class = unreal.load_class(None, "/Script/SLFConversion.PDA_Action")
if not cpp_class:
    lines.append("ERROR: Could not load C++ PDA_Action class!")
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(lines))
    raise Exception("Cannot load C++ class")

lines.append(f"\n1. C++ class loaded: {cpp_class.get_name()}")
lines.append(f"   Path: {cpp_class.get_path_name()}")

# Load the Blueprint
bp_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/PDA_Action"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if not bp:
    lines.append("ERROR: Could not load Blueprint PDA_Action!")
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(lines))
    raise Exception("Cannot load Blueprint")

lines.append(f"\n2. Blueprint loaded: {bp}")

# Check current parent
gen_class = bp.generated_class()
lines.append(f"   Generated class: {gen_class.get_name()}")
if hasattr(gen_class, 'get_super_class'):
    super_class = gen_class.get_super_class()
    if super_class:
        lines.append(f"   Current parent: {super_class.get_name()} ({super_class.get_path_name()})")
    else:
        lines.append("   Current parent: None")

# Perform reparent
lines.append(f"\n3. Calling reparent_blueprint()...")
try:
    result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
    lines.append(f"   Result: {result}")

    if result:
        lines.append("   Reparent succeeded!")

        # Compile the blueprint
        lines.append(f"\n4. Compiling blueprint...")
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        lines.append("   Compiled!")

        # Save the blueprint
        lines.append(f"\n5. Saving blueprint...")
        unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
        lines.append("   Saved!")

        # Verify new parent
        gen_class2 = bp.generated_class()
        if hasattr(gen_class2, 'get_super_class'):
            super_class2 = gen_class2.get_super_class()
            if super_class2:
                lines.append(f"\n6. New parent: {super_class2.get_name()} ({super_class2.get_path_name()})")
                if "SLFConversion" in super_class2.get_path_name():
                    lines.append("\n   SUCCESS: Now inherits from C++ UPDA_Action!")
                else:
                    lines.append("\n   WARNING: Parent changed but not to C++ class")
    else:
        lines.append("   Reparent returned False!")
except Exception as e:
    lines.append(f"   ERROR: {e}")

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print(f"Output written to {OUTPUT_FILE}")
