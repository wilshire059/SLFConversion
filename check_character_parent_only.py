"""
CHECK ONLY - Does not modify anything.
Run in Unreal Editor: py check_character_parent_only.py
"""
import unreal

def log(msg):
    unreal.log_warning(msg)

def main():
    log("=" * 70)
    log("CHECKING CHARACTER BLUEPRINT PARENT CLASSES (READ-ONLY)")
    log("=" * 70)

    blueprints = [
        "/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter",
        "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character",
    ]

    for bp_path in blueprints:
        bp_name = bp_path.split("/")[-1]
        bp = unreal.load_asset(bp_path)

        if not bp:
            log(f"{bp_name}: [ERROR] Could not load")
            continue

        # Use generated_class() to get the class, then check its parent
        gen_class = bp.generated_class()
        if not gen_class:
            log(f"{bp_name}: [ERROR] No generated class")
            continue

        # Get class name to check parent in the hierarchy
        class_name = gen_class.get_name()
        class_path = gen_class.get_path_name()

        log(f"")
        log(f"{bp_name}:")
        log(f"  Generated class: {class_name}")
        log(f"  Path: {class_path}")

        # Try to use SLFAutomationLibrary if available
        try:
            parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
            parent_name = parent.get_name() if parent else "None"
            parent_path = parent.get_path_name() if parent else "None"
            log(f"  Parent: {parent_name}")
            log(f"  Parent Path: {parent_path}")
        except:
            # Fallback: check the class name pattern
            log(f"  Parent: (could not determine via Python API)")
            parent_name = class_name

        # Check if C++ based on known patterns
        if "SLFSoulslikeCharacter" in class_path:
            log(f"  Status: [OK] Parented to C++ SLFSoulslikeCharacter")
        elif "SLFBaseCharacter" in class_path:
            log(f"  Status: [OK] Parented to C++ SLFBaseCharacter")
        elif "_C" in class_name and "Game" in class_path:
            log(f"  Status: [WARNING] Blueprint class - parent might need fix")
        else:
            log(f"  Status: [INFO] Cannot determine parent hierarchy")

    log("")
    log("=" * 70)
    log("CHECK COMPLETE - No changes made")
    log("=" * 70)

if __name__ == "__main__":
    main()
