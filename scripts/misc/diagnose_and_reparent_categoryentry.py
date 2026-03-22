"""
Diagnose and reparent W_Settings_CategoryEntry to C++ class.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/diagnose_and_reparent_categoryentry.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("Diagnose and Reparent W_Settings_CategoryEntry")
    unreal.log_warning("=" * 60)

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry"
    cpp_class_path = "/Script/SLFConversion.W_Settings_CategoryEntry"

    # Load the Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_warning(f"FAIL: Could not load {bp_path}")
        return

    unreal.log_warning(f"Loaded Blueprint: {bp.get_name()}")

    # Get current parent info
    gen_class = bp.generated_class()
    if gen_class:
        unreal.log_warning(f"Generated class: {gen_class.get_name()}")
        unreal.log_warning(f"Generated class path: {gen_class.get_path_name()}")

    # Get parent class - use generated_class().get_super_class() equivalent
    parent_class = None
    try:
        # Walk up the class hierarchy
        if gen_class:
            super_class = unreal.SystemLibrary.get_class_display_name(gen_class)
            unreal.log_warning(f"Generated class display name: {super_class}")
    except Exception as e:
        unreal.log_warning(f"Could not get parent info: {e}")

    # Load the C++ class
    cpp_class = unreal.load_class(None, cpp_class_path)
    if not cpp_class:
        unreal.log_warning(f"FAIL: Could not load C++ class {cpp_class_path}")
        return

    unreal.log_warning(f"C++ class: {cpp_class.get_name()}")
    unreal.log_warning(f"C++ class path: {cpp_class.get_path_name()}")

    # Check if already reparented by checking if generated class is child of C++ class
    if gen_class:
        try:
            is_child = gen_class.is_child_of(cpp_class)
            unreal.log_warning(f"Generated class is_child_of C++ class: {is_child}")
            if is_child:
                unreal.log_warning("ALREADY REPARENTED: Blueprint is already using C++ parent!")
                return
        except Exception as e:
            unreal.log_warning(f"Could not check inheritance: {e}")

    # Try reparenting
    unreal.log_warning("")
    unreal.log_warning("Attempting reparent...")

    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        unreal.log_warning(f"Reparent result: {result}")

        if result:
            # Compile and save
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            unreal.log_warning("Compiled")

            unreal.EditorAssetLibrary.save_asset(bp_path)
            unreal.log_warning("Saved")

            unreal.log_warning("SUCCESS: Reparented to C++!")
        else:
            unreal.log_warning("FAIL: Reparent returned false")

            # Check what the parent is after the failed attempt
            gen_class2 = bp.generated_class()
            if gen_class2:
                is_child2 = gen_class2.is_child_of(cpp_class)
                unreal.log_warning(f"After reparent attempt - generated class is_child_of C++: {is_child2}")

    except Exception as e:
        unreal.log_warning(f"ERROR during reparent: {e}")

    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning("Done")
    unreal.log_warning("=" * 60)

if __name__ == "__main__":
    main()
