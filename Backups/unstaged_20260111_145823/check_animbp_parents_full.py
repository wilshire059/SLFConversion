# check_animbp_parents_full.py
# Check actual parent class of AnimBPs in the project

import unreal

def log(msg):
    unreal.log_warning(f"[ParentCheck] {msg}")

def main():
    log("=" * 60)
    log("AnimBP Parent Class Check")
    log("=" * 60)

    animbp_paths = [
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
    ]

    for path in animbp_paths:
        bp = unreal.load_asset(path)
        if not bp:
            log(f"[ERROR] Failed to load: {path}")
            continue

        name = path.split("/")[-1]
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)

        # Check if parent is C++ or Blueprint
        if "Script/Engine" in parent:
            parent_type = "Engine C++ (UAnimInstance)"
        elif "Script/SLFConversion" in parent:
            parent_type = "Our C++ class"
        else:
            parent_type = "Unknown"

        log(f"{name}:")
        log(f"  Parent: {parent}")
        log(f"  Type: {parent_type}")

        # Get compile status
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(bp)
        has_errors = "Errors: 0" not in errors
        has_warnings = "Warnings: 0" not in errors
        if has_errors:
            log(f"  Status: [FAIL] Has compile errors")
        elif has_warnings:
            log(f"  Status: [WARN] Has warnings")
        else:
            log(f"  Status: [OK] Compiles clean")

    log("")
    log("=" * 60)
    log("Summary:")
    log("If parent is 'Engine C++ (UAnimInstance)', Blueprint logic handles updates.")
    log("If parent is 'Our C++ class', C++ NativeUpdateAnimation handles updates.")
    log("=" * 60)

if __name__ == "__main__":
    main()
