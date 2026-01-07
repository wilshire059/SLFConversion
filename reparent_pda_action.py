# reparent_pda_action.py
# Reparent the PDA_Action Blueprint to the C++ UPDA_Action class
# Run AFTER extract_action_data.py

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/check_output.txt"

lines = []
lines.append("=" * 60)
lines.append("REPARENTING PDA_Action TO C++ UPDA_Action")
lines.append("=" * 60)

try:
    # Load the Blueprint
    bp_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/PDA_Action"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    lines.append(f"\n1. Blueprint: {bp}")

    if bp is None:
        lines.append("   ERROR: Could not load Blueprint")
    else:
        # Get current parent
        gen_class = bp.generated_class()
        lines.append(f"   Generated class: {gen_class}")
        if hasattr(gen_class, 'get_super_struct'):
            current_parent = gen_class.get_super_struct()
            lines.append(f"   Current parent: {current_parent}")

        # Get the C++ parent class
        # Note: In Python, use PDA_Action (without U prefix) for the class path
        cpp_class = unreal.load_class(None, "/Script/SLFConversion.PDA_Action")
        lines.append(f"\n2. C++ target class: {cpp_class}")

        if cpp_class is None:
            lines.append("   ERROR: Could not load C++ class /Script/SLFConversion.PDA_Action")
            lines.append("   Trying alternate path...")
            cpp_class = unreal.load_class(None, "/Script/SLFConversion.UPDA_Action")
            lines.append(f"   Alternate result: {cpp_class}")

        if cpp_class is None:
            lines.append("   ERROR: Could not load C++ class")
        else:
            # Reparent the Blueprint
            lines.append(f"\n3. Attempting reparent...")
            result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
            lines.append(f"   Reparent result: {result}")

            # Compile the Blueprint
            lines.append(f"\n4. Compiling Blueprint...")
            compile_result = unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            lines.append(f"   Compile result: {compile_result}")

            # Save the Blueprint
            lines.append(f"\n5. Saving Blueprint...")
            save_result = unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
            lines.append(f"   Save result: {save_result}")

            # Verify parent
            lines.append(f"\n6. Verification:")
            bp_reloaded = unreal.EditorAssetLibrary.load_asset(bp_path)
            new_gen_class = bp_reloaded.generated_class()
            lines.append(f"   New generated class: {new_gen_class}")
            if hasattr(new_gen_class, 'get_super_struct'):
                new_parent = new_gen_class.get_super_struct()
                lines.append(f"   New parent class: {new_parent}")

            lines.append("")
            lines.append("=" * 60)
            lines.append("REPARENT COMPLETE")
            lines.append("=" * 60)
            lines.append("Next: Run apply_action_data.py to restore any lost data")

except Exception as e:
    import traceback
    lines.append(f"\nERROR: {e}")
    lines.append(f"Traceback: {traceback.format_exc()}")

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print(f"Output written to {OUTPUT_FILE}")
