#!/usr/bin/env python3
"""Add validation phase to run_migration.py"""

migration_path = 'C:/scripts/SLFConversion/run_migration.py'

with open(migration_path, 'r', encoding='utf-8') as f:
    content = f.read()

old_summary = '''    print("Compile: " + str(compile_ok) + " ok, " + str(compile_fail) + " failed")

    # Summary
    print("")
    print("=" * 60)
    print("SUMMARY")
    print("=" * 60)
    print("Total Blueprints: " + str(len(all_bps)))
    print("Phase 0: priority_ans=" + str(priority_cleared) + ", priority_chars=" + str(priority_char_cleared) + ", anim_bps=" + str(anim_bp_cleared) + ", npc_chars=" + str(npc_cleared))
    print("Phase 1: regular=" + str(cleared) + ", keep_vars=" + str(keep_cleared) + ", interfaces=" + str(iface_cleared) + ", data_assets=" + str(data_cleared))
    print("Reparent: " + str(reparent_ok) + " ok, " + str(reparent_fail) + " failed")
    print("Saved: " + str(saved))
    print("Compile: " + str(compile_ok) + " ok, " + str(compile_fail) + " failed")
    print("=" * 60)

run()'''

new_summary = '''    print("Compile: " + str(compile_ok) + " ok, " + str(compile_fail) + " failed")

    # =========================================================================
    # PHASE 6: POST-MIGRATION VALIDATION
    # =========================================================================
    print("")
    print("=" * 60)
    print("PHASE 6: POST-MIGRATION VALIDATION")
    print("=" * 60)

    # Validate key components have expected properties
    validation_results = []

    # Components to validate with their key properties
    VALIDATION_TARGETS = {
        "/Game/SoulslikeFramework/Blueprints/Components/AC_ActionManager": ["Actions", "AvailableActions", "StaminaRegenDelay"],
        "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager": ["Stats", "StatTable"],
        "/Game/SoulslikeFramework/Blueprints/Components/AC_CombatManager": ["CurrentTarget", "LockedTarget"],
        "/Game/SoulslikeFramework/Blueprints/Components/AC_InputBuffer": ["BufferQueue", "BufferOpen"],
    }

    for bp_path, properties in VALIDATION_TARGETS.items():
        try:
            bp = unreal.EditorAssetLibrary.load_asset(bp_path)
            if not bp:
                validation_results.append(("FAIL", bp_path, "Could not load Blueprint"))
                continue

            gen_class = bp.generated_class()
            if not gen_class:
                validation_results.append(("FAIL", bp_path, "No generated class"))
                continue

            cdo = unreal.get_default_object(gen_class)
            if not cdo:
                validation_results.append(("FAIL", bp_path, "Could not get CDO"))
                continue

            # Check each property exists
            for prop_name in properties:
                try:
                    val = cdo.get_editor_property(prop_name)
                    validation_results.append(("OK", bp_path.split("/")[-1], prop_name + " accessible"))
                except Exception as e:
                    validation_results.append(("FAIL", bp_path.split("/")[-1], prop_name + " error: " + str(e)))

        except Exception as e:
            validation_results.append(("FAIL", bp_path, "Exception: " + str(e)))

    # Print validation results
    val_ok = sum(1 for r in validation_results if r[0] == "OK")
    val_fail = sum(1 for r in validation_results if r[0] == "FAIL")

    for status, component, msg in validation_results:
        if status == "FAIL":
            print("  FAIL: " + component + " - " + msg)

    print("Validation: " + str(val_ok) + " OK, " + str(val_fail) + " FAIL")

    # Summary
    print("")
    print("=" * 60)
    print("SUMMARY")
    print("=" * 60)
    print("Total Blueprints: " + str(len(all_bps)))
    print("Phase 0: priority_ans=" + str(priority_cleared) + ", priority_chars=" + str(priority_char_cleared) + ", anim_bps=" + str(anim_bp_cleared) + ", npc_chars=" + str(npc_cleared))
    print("Phase 1: regular=" + str(cleared) + ", keep_vars=" + str(keep_cleared) + ", interfaces=" + str(iface_cleared) + ", data_assets=" + str(data_cleared))
    print("Reparent: " + str(reparent_ok) + " ok, " + str(reparent_fail) + " failed")
    print("Saved: " + str(saved))
    print("Compile: " + str(compile_ok) + " ok, " + str(compile_fail) + " failed")
    print("Validation: " + str(val_ok) + " OK, " + str(val_fail) + " FAIL")
    print("=" * 60)

run()'''

if old_summary in content:
    content = content.replace(old_summary, new_summary)
    with open(migration_path, 'w', encoding='utf-8') as f:
        f.write(content)
    print('Successfully added validation phase to run_migration.py')
else:
    print('Could not find expected content in run_migration.py - may already be updated')
