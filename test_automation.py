# test_automation.py - Test if SLFAutomationLibrary is accessible

import unreal

print("=" * 60)
print("Testing SLFAutomationLibrary access")
print("=" * 60)

# Check if the class exists
try:
    lib = unreal.SLFAutomationLibrary
    print(f"SLFAutomationLibrary found: {lib}")

    # Check what methods are available
    methods = [m for m in dir(lib) if not m.startswith('_')]
    print(f"Available methods: {methods}")

    # Try to call the function
    print("\nCalling apply_all_status_effect_rank_info()...")
    result = lib.apply_all_status_effect_rank_info()
    print(f"Result: {result}")

except Exception as e:
    print(f"ERROR: {e}")
    import traceback
    traceback.print_exc()

print("=" * 60)
