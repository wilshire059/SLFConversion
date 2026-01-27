"""
Comprehensive validation of chaos destruction migration using C++ functions.
"""
import unreal

def main():
    unreal.log_warning("=== CHAOS DESTRUCTION MIGRATION VALIDATION ===")
    unreal.log_warning("")

    # Run comprehensive validation
    result = unreal.SLFAutomationLibrary.validate_chaos_destruction_migration()
    unreal.log_warning(f"Validation Result:\n{result}")

    unreal.log_warning("")
    unreal.log_warning("=== VALIDATION COMPLETE ===")

if __name__ == "__main__":
    main()
