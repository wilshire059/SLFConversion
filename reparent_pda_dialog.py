"""
Reparent PDA_Dialog Blueprint to UPDA_Dialog C++ class.
This fixes the dialog system by ensuring DA_ExampleDialog assets can be cast to UPDA_Dialog.
"""
import unreal

# Assets to reparent
REPARENT_MAP = {
    "/Game/SoulslikeFramework/Data/Dialog/PDA_Dialog": "/Script/SLFConversion.PDA_Dialog",
    "/Game/SoulslikeFramework/Data/Vendor/PDA_Vendor": "/Script/SLFConversion.PDA_Vendor",
}

def main():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("REPARENTING DIALOG AND VENDOR PDA TO C++")
    unreal.log_warning("="*70 + "\n")

    success_count = 0
    fail_count = 0

    for bp_path, cpp_path in REPARENT_MAP.items():
        bp_name = bp_path.split("/")[-1]
        cpp_name = cpp_path.split(".")[-1]

        unreal.log_warning(f"\n=== {bp_name} -> {cpp_name} ===")

        # Load the Blueprint
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"  [FAIL] Could not load Blueprint: {bp_path}")
            fail_count += 1
            continue

        unreal.log_warning(f"  [OK] Loaded Blueprint")

        # Check current parent
        gen_class = bp.generated_class()
        if gen_class:
            # Get parent class
            parent_name = "Unknown"
            # Can't use get_super_class directly, but we can check the parent via other means
            unreal.log_warning(f"  Current class: {gen_class.get_name()}")

        # Use SLFAutomationLibrary to reparent
        try:
            result = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_path)
            if result:
                unreal.log_warning(f"  [OK] Reparented to {cpp_name}")

                # Compile and save
                compile_result = unreal.SLFAutomationLibrary.compile_and_save(bp)
                if compile_result:
                    unreal.log_warning(f"  [OK] Compiled and saved")
                    success_count += 1
                else:
                    unreal.log_warning(f"  [WARN] Reparented but compile failed")
                    success_count += 1  # Still count as success since reparent worked
            else:
                unreal.log_warning(f"  [FAIL] Reparent failed")
                fail_count += 1
        except Exception as e:
            unreal.log_warning(f"  [FAIL] Exception: {e}")
            fail_count += 1

    unreal.log_warning(f"\n" + "="*70)
    unreal.log_warning(f"SUMMARY: {success_count} succeeded, {fail_count} failed")
    unreal.log_warning("="*70 + "\n")

if __name__ == "__main__":
    main()
