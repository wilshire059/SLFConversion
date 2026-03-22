import unreal

output = []
def log(msg):
    print(msg)
    output.append(str(msg))

automation = unreal.SLFAutomationLibrary

log("=" * 70)
log("VERIFYING ALL FIXES")
log("=" * 70)

# 1. Check B_SequenceActor
log("\n1. B_SequenceActor:")
bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if bp:
    parent = automation.get_blueprint_parent_class(bp)
    log(f"   Parent: {parent}")
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            try:
                seq = cdo.get_editor_property("sequence_to_play")
                log(f"   sequence_to_play: {seq.get_name() if seq else 'None'}")
            except Exception as e:
                log(f"   ERROR: {e}")
    errors = automation.get_blueprint_compile_errors(bp)
    if "Errors: 0" in errors:
        log("   Compile: OK")
    else:
        log(f"   Compile: ERRORS\n{errors[:200]}")

# 2. Check GI_SoulslikeFramework
log("\n2. GI_SoulslikeFramework:")
gi_path = "/Game/SoulslikeFramework/Blueprints/Global/GI_SoulslikeFramework"
gi_bp = unreal.EditorAssetLibrary.load_asset(gi_path)
if gi_bp:
    parent = automation.get_blueprint_parent_class(gi_bp)
    log(f"   Parent: {parent}")
    gen_class = gi_bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            try:
                val = cdo.get_editor_property("first_time_on_demo_level")
                log(f"   first_time_on_demo_level: {val}")
            except Exception as e:
                log(f"   ERROR accessing property: {e}")
    errors = automation.get_blueprint_compile_errors(gi_bp)
    if "Errors: 0" in errors:
        log("   Compile: OK")
    else:
        log(f"   Compile: ERRORS\n{errors[:200]}")

# 3. Check Level Blueprint
log("\n3. Level Blueprint (L_Demo_Showcase):")
level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
editor_subsystem = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
if editor_subsystem:
    editor_subsystem.load_level(level_path)
    world = unreal.EditorLevelLibrary.get_editor_world()
    if world:
        level = world.get_current_level()
        if level:
            level_bp = level.get_level_script_blueprint()
            if level_bp:
                errors = automation.get_blueprint_compile_errors(level_bp)
                if "Errors: 0" in errors:
                    log("   Compile: OK")
                else:
                    log(f"   Compile: ERRORS/WARNINGS\n{errors[:300]}")

# 4. Check AC_AI_CombatManager
log("\n4. AC_AI_CombatManager (status effects):")
cm_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_CombatManager"
cm_bp = unreal.EditorAssetLibrary.load_asset(cm_path)
if cm_bp:
    parent = automation.get_blueprint_parent_class(cm_bp)
    log(f"   Parent: {parent}")
    errors = automation.get_blueprint_compile_errors(cm_bp)
    if "Errors: 0" in errors:
        log("   Compile: OK")
    else:
        log(f"   Compile: ERRORS\n{errors[:200]}")

# 5. Check AC_AI_Boss
log("\n5. AC_AI_Boss (phase sequences):")
boss_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_Boss"
boss_bp = unreal.EditorAssetLibrary.load_asset(boss_path)
if boss_bp:
    parent = automation.get_blueprint_parent_class(boss_bp)
    log(f"   Parent: {parent}")
    errors = automation.get_blueprint_compile_errors(boss_bp)
    if "Errors: 0" in errors:
        log("   Compile: OK")
    else:
        log(f"   Compile: ERRORS\n{errors[:200]}")

log("\n" + "=" * 70)
log("VERIFICATION COMPLETE")
log("=" * 70)

# Write output
with open("C:/scripts/SLFConversion/migration_cache/all_fixes_verify.txt", 'w') as f:
    f.write('\n'.join(output))
