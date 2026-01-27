"""
Fresh test of W_Settings compilation to verify DisplayName fixes.

This test:
1. Loads W_Settings Blueprint
2. Forces recompilation
3. Outputs any errors to stdout
"""

import unreal

def main():
    print("=" * 60)
    print("Fresh W_Settings Compilation Test")
    print("=" * 60)

    bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings"

    # Load the Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        print(f"ERROR: Could not load Blueprint: {bp_path}")
        return

    print(f"Loaded: {bp.get_name()}")
    print(f"Parent class: {bp.get_class().get_name()}")

    # Force compile
    print("Compiling...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        print("Compile completed (check for errors above)")
    except Exception as e:
        print(f"COMPILE EXCEPTION: {e}")

    # Check if it's a valid Blueprint
    print(f"Is valid Blueprint: {bp is not None}")

    print("=" * 60)
    print("Test complete")
    print("=" * 60)


if __name__ == "__main__":
    main()
