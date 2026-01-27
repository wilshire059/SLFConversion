# test_automation2.py - Test with UE logging

import unreal

unreal.log_warning("=" * 60)
unreal.log_warning("Testing SLFAutomationLibrary access")
unreal.log_warning("=" * 60)

# Check if the class exists
try:
    lib = unreal.SLFAutomationLibrary
    unreal.log_warning(f"SLFAutomationLibrary found: {lib}")

    # Check what methods are available
    methods = [m for m in dir(lib) if not m.startswith('_')]
    unreal.log_warning(f"Available methods: {methods}")

    # Try to call the function
    unreal.log_warning("Calling apply_all_status_effect_rank_info()...")
    result = lib.apply_all_status_effect_rank_info()
    unreal.log_warning(f"Result: {result}")

except Exception as e:
    unreal.log_error(f"ERROR: {e}")
    import traceback
    unreal.log_error(traceback.format_exc())

unreal.log_warning("=" * 60)
