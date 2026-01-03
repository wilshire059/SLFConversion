"""
migrate_all_components.py

Master migration script - runs all component migrations in the correct order.

Usage (in Unreal Editor Python console):
    exec(open(r"C:\scripts\SLFConversion\migrate_all_components.py").read())

Prerequisites:
    1. C++ compiled:
       - UInputBufferComponent
       - UBuffManagerComponent
       - UActionManagerComponent
       - UStatManagerComponent
       - UBlueprintFixerLibrary
    2. Content folder restored from backup (clean state)

Migration Order:
    1. InputBuffer - Input queueing for combat
    2. BuffManager - Buff/debuff management
    3. ActionManager - Action execution and movement
    4. StatManager - Character stats (depends on ActionManager for stamina)

After migration:
    - AC_InputBuffer inherits from UInputBufferComponent
    - AC_BuffManager inherits from UBuffManagerComponent
    - AC_ActionManager inherits from UActionManagerComponent
    - AC_StatManager inherits from UStatManagerComponent
    - All Blueprints become "shell" classes with no logic
"""

import unreal
import os

# Script paths
SCRIPT_DIR = r"C:\scripts\SLFConversion"

MIGRATION_SCRIPTS = [
    ("InputBuffer", "migrate_inputbuffer_automated.py"),
    ("BuffManager", "migrate_buffmanager_automated.py"),
    ("ActionManager", "migrate_actionmanager_automated.py"),
    ("StatManager", "migrate_statmanager_automated.py"),
]

# C++ classes to check
CPP_CLASSES = [
    ("InputBufferComponent", "/Script/SLFConversion.InputBufferComponent"),
    ("BuffManagerComponent", "/Script/SLFConversion.BuffManagerComponent"),
    ("ActionManagerComponent", "/Script/SLFConversion.ActionManagerComponent"),
    ("StatManagerComponent", "/Script/SLFConversion.StatManagerComponent"),
]


def log_banner(title):
    """Print a large banner."""
    print()
    print("#" * 70)
    print("#")
    print(f"#   {title}")
    print("#")
    print("#" * 70)
    print()


def check_all_prerequisites():
    """Check that all C++ classes are compiled."""
    log_banner("PREREQUISITE CHECK - ALL C++ CLASSES")

    all_ok = True

    # Check BlueprintFixerLibrary
    try:
        _ = unreal.BlueprintFixerLibrary
        print("[OK] BlueprintFixerLibrary")
    except AttributeError:
        print("[FAIL] BlueprintFixerLibrary - Compile C++ first!")
        all_ok = False

    # Check all component classes
    for name, path in CPP_CLASSES:
        cpp_class = unreal.load_class(None, path)
        if cpp_class:
            print(f"[OK] {name}")
        else:
            print(f"[FAIL] {name} - Compile C++ first!")
            all_ok = False

    return all_ok


def run_migration_script(name, script_filename):
    """Run a single migration script."""
    log_banner(f"MIGRATING: {name}")

    script_path = os.path.join(SCRIPT_DIR, script_filename)

    if not os.path.exists(script_path):
        print(f"[FAIL] Script not found: {script_path}")
        return False

    try:
        # Execute the script
        with open(script_path, 'r') as f:
            script_content = f.read()

        # The script will print its own output
        exec(script_content)

        return True

    except Exception as e:
        print(f"[FAIL] Error running {script_filename}: {e}")
        import traceback
        traceback.print_exc()
        return False


def verify_all_migrations():
    """Verify all components were migrated."""
    log_banner("FINAL VERIFICATION")

    bp_paths = [
        ("InputBuffer", "/Game/SoulslikeFramework/Blueprints/Components/AC_InputBuffer", "InputBufferComponent"),
        ("BuffManager", "/Game/SoulslikeFramework/Blueprints/Components/AC_BuffManager", "BuffManagerComponent"),
        ("ActionManager", "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager", "ActionManagerComponent"),
        ("StatManager", "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager", "StatManagerComponent"),
    ]

    all_ok = True

    for name, bp_path, expected_parent in bp_paths:
        bp = unreal.load_asset(bp_path)
        if not bp:
            print(f"[FAIL] {name}: Could not load Blueprint")
            all_ok = False
            continue

        try:
            parent = bp.parent_class
            parent_name = parent.get_name() if parent else "None"
        except:
            parent_name = "Unknown"

        if expected_parent in parent_name:
            print(f"[OK] {name}: Parent is {parent_name}")
        else:
            print(f"[FAIL] {name}: Expected {expected_parent}, got {parent_name}")
            all_ok = False

    return all_ok


def run_all_migrations():
    """Main function - run all migrations in order."""
    log_banner("MASTER MIGRATION SCRIPT")
    print("This script will migrate ALL components to C++ in order:")
    print()
    for i, (name, _) in enumerate(MIGRATION_SCRIPTS, 1):
        print(f"  {i}. {name}")
    print()
    print("Each component's Blueprint becomes a 'shell' class that")
    print("inherits all functionality from C++.")

    # Check prerequisites
    if not check_all_prerequisites():
        print("\n[ABORT] Not all C++ classes are compiled!")
        print("\nPlease compile C++ first:")
        print('  "C:\\Program Files\\Epic Games\\UE_5.7\\Engine\\Build\\BatchFiles\\Build.bat" SLFConversionEditor Win64 Development -Project="C:\\scripts\\SLFConversion\\SLFConversion.uproject" -WaitMutex -FromMsBuild')
        return False

    # Run each migration
    results = {}
    for name, script in MIGRATION_SCRIPTS:
        success = run_migration_script(name, script)
        results[name] = success

        if not success:
            print(f"\n[WARN] {name} migration had issues - continuing anyway...")

    # Final verification
    all_verified = verify_all_migrations()

    # Summary
    log_banner("MIGRATION SUMMARY")

    print("Migration Results:")
    for name, success in results.items():
        status = "[OK]" if success else "[FAIL]"
        print(f"  {status} {name}")

    print()
    if all_verified:
        print("[SUCCESS] All components migrated successfully!")
        print()
        print("Next steps:")
        print("  1. Compile all Blueprints (right-click > Compile)")
        print("  2. Check Output Log for any warnings")
        print("  3. Test in Play mode:")
        print("     - Combat actions (attack, dodge)")
        print("     - Stamina drain during sprint")
        print("     - Buff application/removal")
        print("     - Stat changes and level up")
        print("  4. Save all assets")
    else:
        print("[PARTIAL] Some migrations had issues")
        print()
        print("Debug steps:")
        print("  1. Check Output Log for 'LogBlueprintFixer' messages")
        print("  2. Run individual migration scripts for failed components")
        print("  3. Compare against JSON exports in:")
        print("     C:/scripts/SLFConversion_Migration/Backups/SoulsClean/Exports/BlueprintDNA/")

    return all_verified


# Run all migrations
run_all_migrations()
