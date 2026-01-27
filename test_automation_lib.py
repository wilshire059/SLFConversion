"""
Test if SLFAutomationLibrary functions are available
"""
import unreal

unreal.log("=== Testing SLFAutomationLibrary ===")

# List available methods
try:
    lib = unreal.SLFAutomationLibrary
    unreal.log(f"SLFAutomationLibrary found: {lib}")

    # List methods
    methods = [m for m in dir(lib) if not m.startswith('_')]
    unreal.log(f"Available methods: {methods[:20]}...")

    # Check if our function exists
    if hasattr(lib, 'apply_weapon_stat_data'):
        unreal.log("apply_weapon_stat_data EXISTS")
    else:
        unreal.log_warning("apply_weapon_stat_data NOT FOUND")

except Exception as e:
    unreal.log_error(f"Error: {e}")

unreal.log("=== Test Complete ===")
