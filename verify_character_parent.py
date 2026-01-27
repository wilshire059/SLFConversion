"""
Verify B_Soulslike_Character's current parent class.
"""
import unreal

def log(msg):
    unreal.log_warning(msg)

def main():
    log("=" * 70)
    log("VERIFYING B_SOULSLIKE_CHARACTER PARENT CLASS")
    log("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    bp = unreal.load_asset(bp_path)

    if not bp:
        log(f"[ERROR] Could not load {bp_path}")
        return

    gen_class = bp.generated_class()
    if not gen_class:
        log("[ERROR] No generated class")
        return

    log(f"Blueprint: {bp.get_name()}")
    log(f"Generated Class: {gen_class.get_name()}")
    log(f"Generated Class Path: {gen_class.get_path_name()}")

    # Try to get parent info using SLFAutomationLibrary
    try:
        parent_info = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"Parent Class (via SLFAutomationLibrary): {parent_info}")
    except Exception as e:
        log(f"SLFAutomationLibrary error: {e}")

    # Check if it's pointing to C++ class
    class_path = gen_class.get_path_name()
    if "SLFSoulslikeCharacter" in class_path or "/Script/SLFConversion" in class_path:
        log("[OK] Blueprint appears to be parented to C++ class")
    elif "/Game/" in class_path:
        log("[WARNING] Blueprint appears to be parented to another Blueprint")
    else:
        log(f"[INFO] Parent type unclear from path: {class_path}")

    log("=" * 70)

if __name__ == "__main__":
    main()
