"""
Test if SLFAutomationLibrary functions are available - write to file
"""
import unreal

output_file = "C:/scripts/SLFConversion/automation_test_output.txt"

with open(output_file, 'w') as f:
    f.write("=== Testing SLFAutomationLibrary ===\n")

    # List available methods
    try:
        lib = unreal.SLFAutomationLibrary
        f.write(f"SLFAutomationLibrary found: {lib}\n")

        # List methods
        methods = [m for m in dir(lib) if not m.startswith('_')]
        f.write(f"Available methods ({len(methods)} total):\n")
        for m in methods:
            f.write(f"  - {m}\n")

        # Check if our function exists
        if hasattr(lib, 'apply_weapon_stat_data'):
            f.write("\napply_weapon_stat_data EXISTS\n")
        else:
            f.write("\napply_weapon_stat_data NOT FOUND\n")

    except Exception as e:
        f.write(f"Error: {e}\n")

    f.write("\n=== Test Complete ===\n")

print(f"Output written to {output_file}")
