"""
Fix ANS_ToggleChaosField parent using C++ ForceReparentAndValidate.
"""
import unreal

def main():
    unreal.log_warning("=== FIXING ANS_ToggleChaosField PARENT ===")
    unreal.log_warning("")

    bp_path = "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_ToggleChaosField"
    new_parent = "/Script/SLFConversion.ANS_ToggleChaosField"

    unreal.log_warning(f"Blueprint: {bp_path}")
    unreal.log_warning(f"New Parent: {new_parent}")
    unreal.log_warning("")

    # Use C++ function to reparent and validate
    result = unreal.SLFAutomationLibrary.force_reparent_and_validate(bp_path, new_parent)
    unreal.log_warning(f"Reparent Result:\n{result}")

    unreal.log_warning("")

    # Run validation again to confirm
    unreal.log_warning("=== VALIDATING AFTER FIX ===")
    validation = unreal.SLFAutomationLibrary.validate_chaos_destruction_migration()
    unreal.log_warning(f"Validation Result:\n{validation}")

    unreal.log_warning("")
    unreal.log_warning("=== DONE ===")

if __name__ == "__main__":
    main()
