"""
Verify that stat Blueprints are now children of UB_Stat
"""

import unreal

def verify_stat_reparenting():
    """Verify that stat Blueprints now inherit from C++ stat classes."""

    # Write results to file
    results = []

    # Test paths to verify
    test_paths = [
        "/Game/SoulslikeFramework/Data/Stats/B_Stat",
        "/Game/SoulslikeFramework/Data/Stats/Secondary/B_HP",
        "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Fire",
        "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Physical",
    ]

    # Load UB_Stat C++ class
    cpp_stat_class = unreal.load_class(None, "/Script/SLFConversion.B_Stat")
    if not cpp_stat_class:
        results.append("ERROR: Failed to load UB_Stat class from C++!")
        with open("C:/scripts/SLFConversion/verify_results.txt", "w") as f:
            f.write("\n".join(results))
        return

    results.append("=" * 60)
    results.append("VERIFYING STAT REPARENTING")
    results.append("=" * 60)
    results.append(f"C++ UB_Stat class: {cpp_stat_class.get_name()}")

    for bp_path in test_paths:
        bp_asset = unreal.load_asset(bp_path)
        if not bp_asset:
            results.append(f"ERROR: Failed to load: {bp_path}")
            continue

        # Get the Blueprint's generated class
        gen_class = bp_asset.generated_class()
        if not gen_class:
            results.append(f"ERROR: {bp_path}: No generated class!")
            continue

        # Get parent class
        parent_class = None
        parent_name = "Unknown"
        try:
            parent_class = bp_asset.get_editor_property('parent_class')
            parent_name = parent_class.get_name() if parent_class else "None"
        except:
            pass

        # Check if it's a child of UB_Stat by walking the parent chain
        is_child = False
        check_class = parent_class
        parent_chain = []
        while check_class:
            parent_chain.append(check_class.get_name())
            if check_class == cpp_stat_class:
                is_child = True
                break
            # Get next parent (for UClass, use super_struct)
            try:
                check_class = check_class.get_super_struct()
            except:
                break

        status = "OK" if is_child else "FAIL"
        results.append(f"[{status}] {bp_path}")
        results.append(f"        Parent: {parent_name}")
        results.append(f"        Generated: {gen_class.get_name()}")
        results.append(f"        Parent chain: {' -> '.join(parent_chain[:5])}")

    results.append("=" * 60)

    # Write results to file
    with open("C:/scripts/SLFConversion/verify_results.txt", "w") as f:
        f.write("\n".join(results))

    # Also log
    for line in results:
        unreal.log(line)


if __name__ == "__main__":
    verify_stat_reparenting()
