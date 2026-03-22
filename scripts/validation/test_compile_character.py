# test_compile_character.py
# Test: Can we compile B_Soulslike_Character after reparenting?
# This isolates the compilation step to see what crashes/errors occur.

import unreal
import time

def run():
    unreal.log_warning("=" * 60)
    unreal.log_warning("TEST: Compile Character Blueprints After Reparenting")
    unreal.log_warning("=" * 60)

    # The two character Blueprints we need
    TEST_BLUEPRINTS = {
        "B_BaseCharacter": {
            "path": "/Game/SoulslikeFramework/Blueprints/_Characters/B_BaseCharacter",
            "cpp": "/Script/SLFConversion.SLFBaseCharacter"
        },
        "B_Soulslike_Character": {
            "path": "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character",
            "cpp": "/Script/SLFConversion.SLFSoulslikeCharacter"
        }
    }

    results = {}

    for name, config in TEST_BLUEPRINTS.items():
        unreal.log_warning("")
        unreal.log_warning("=" * 40)
        unreal.log_warning("Testing: " + name)
        unreal.log_warning("=" * 40)

        try:
            # Step 1: Load the Blueprint
            unreal.log_warning("1. Loading " + name + "...")
            bp = unreal.load_asset(config["path"])
            if not bp:
                unreal.log_warning("   FAILED: Could not load " + config["path"])
                results[name] = "LOAD_FAILED"
                continue
            unreal.log_warning("   Loaded: " + bp.get_name())

            # Step 2: Check current parent
            current_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
            unreal.log_warning("2. Current parent: " + str(current_parent))

            # Step 3: Clear graphs (keep variables)
            unreal.log_warning("3. Clearing graphs (keeping variables)...")
            cleared = unreal.SLFAutomationLibrary.clear_graphs_keep_variables_no_compile(bp)
            unreal.log_warning("   Cleared: " + str(cleared))

            # Step 4: Remove interfaces
            unreal.log_warning("4. Removing interfaces...")
            removed = unreal.SLFAutomationLibrary.remove_implemented_interfaces(bp)
            unreal.log_warning("   Removed " + str(removed) + " interfaces")

            # Step 5: Reparent
            unreal.log_warning("5. Reparenting to " + config["cpp"] + "...")
            success = unreal.SLFAutomationLibrary.reparent_blueprint(bp, config["cpp"])
            if not success:
                unreal.log_warning("   FAILED: Reparenting failed")
                results[name] = "REPARENT_FAILED"
                continue
            unreal.log_warning("   Reparented successfully")

            # Step 6: COMPILE (this is the test!)
            unreal.log_warning("6. COMPILING (this is the test)...")
            try:
                compile_result = unreal.SLFAutomationLibrary.compile_and_save(bp)
                if compile_result:
                    unreal.log_warning("   COMPILE SUCCESS!")
                    results[name] = "SUCCESS"
                else:
                    unreal.log_warning("   COMPILE RETURNED FALSE (errors)")
                    results[name] = "COMPILE_ERRORS"
            except Exception as compile_error:
                unreal.log_warning("   COMPILE EXCEPTION: " + str(compile_error))
                results[name] = "COMPILE_EXCEPTION: " + str(compile_error)

        except Exception as e:
            unreal.log_warning("   EXCEPTION: " + str(e))
            results[name] = "EXCEPTION: " + str(e)

    # Summary
    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning("RESULTS")
    unreal.log_warning("=" * 60)
    for name, result in results.items():
        unreal.log_warning("  " + name + ": " + result)
    unreal.log_warning("=" * 60)

run()
