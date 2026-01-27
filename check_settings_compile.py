"""
Check W_Settings compile errors.

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/check_settings_compile.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal


def check_compile(bp_path):
    """Check Blueprint compile status."""

    unreal.log_warning(f"Loading {bp_path}")

    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Failed to load {bp_path}")
        return

    unreal.log_warning(f"Type: {type(bp)}")

    # Try to compile
    try:
        result = unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log_warning(f"Compile result: {result}")
    except Exception as e:
        unreal.log_error(f"Compile exception: {e}")

    # Check for errors
    gen_class = bp.generated_class()
    if gen_class:
        unreal.log_warning(f"Generated class: {gen_class.get_name()}")
        parent = gen_class.get_super_struct()
        if parent:
            unreal.log_warning(f"Parent class: {parent.get_name()}")
    else:
        unreal.log_error("No generated class - Blueprint likely has errors")


def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("Checking W_Settings Compile Status")
    unreal.log_warning("=" * 70)

    check_compile("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings")

    unreal.log_warning("=" * 70)
    unreal.log_warning("Done!")
    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    main()
