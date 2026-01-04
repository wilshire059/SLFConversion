# reparent_pda_item.py
# Reparent the PDA_Item Blueprint to the C++ UPDA_Item class

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/check_output.txt"

lines = []
lines.append("=" * 60)
lines.append("REPARENTING PDA_Item TO C++ UPDA_Item")
lines.append("=" * 60)

try:
    # Load the Blueprint
    bp_path = "/Game/SoulslikeFramework/Data/PDA_Item"
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    lines.append(f"\n1. Blueprint: {bp}")

    # Get current parent
    gen_class = bp.generated_class()
    lines.append(f"   Generated class: {gen_class}")

    # Get the C++ parent class
    cpp_class = unreal.load_class(None, "/Script/SLFConversion.PDA_Item")
    lines.append(f"\n2. C++ target class: {cpp_class}")

    if cpp_class is None:
        lines.append("   ERROR: Could not load C++ class")
    else:
        # Check if already reparented
        lines.append(f"\n3. Attempting reparent...")

        # Check signature by calling help
        lines.append(f"   reparent_blueprint function: {unreal.BlueprintEditorLibrary.reparent_blueprint}")

        # Call reparent_blueprint
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        lines.append(f"   Result: {result}")

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
        new_gen_class = bp.generated_class()
        lines.append(f"   New generated class: {new_gen_class}")
        if hasattr(new_gen_class, 'get_super_struct'):
            parent = new_gen_class.get_super_struct()
            lines.append(f"   Parent class: {parent}")

except Exception as e:
    import traceback
    lines.append(f"\nERROR: {e}")
    lines.append(f"Traceback: {traceback.format_exc()}")

with open(OUTPUT_FILE, 'w') as f:
    f.write('\n'.join(lines))

print(f"Output written to {OUTPUT_FILE}")
