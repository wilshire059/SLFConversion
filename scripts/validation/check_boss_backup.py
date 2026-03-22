"""
Check boss configuration in bp_only (backup) project.
"""

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/boss_backup_report.txt"
output_lines = []

def log(msg):
    output_lines.append(str(msg))
    unreal.log_warning(str(msg))

def write_output():
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(output_lines))

def check_boss_backup():
    try:
        # Load the boss Blueprint
        boss_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth"
        boss_bp = unreal.EditorAssetLibrary.load_asset(boss_path)

        if not boss_bp:
            log(f"ERROR: Could not load boss Blueprint at {boss_path}")
            return

        log("")
        log("=" * 60)
        log(f"BOSS BACKUP CHECK: {boss_bp.get_name()}")
        log("=" * 60)

        gen_class = boss_bp.generated_class()
        cdo = unreal.get_default_object(gen_class)

        log(f"Class: {gen_class.get_name()}")

        # Check all components
        log("")
        log("--- All CDO Components ---")
        all_comps = cdo.get_components_by_class(unreal.ActorComponent)
        if all_comps:
            for comp in all_comps:
                comp_name = comp.get_name()
                comp_class = comp.get_class().get_name()
                log(f"  {comp_name} ({comp_class})")

                # Check boss component phases
                if "Boss" in comp_class:
                    log(f"    >>> BOSS COMPONENT <<<")
                    try:
                        phases = comp.get_editor_property("phases")
                        log(f"    Phases: {len(phases) if phases else 0}")
                        if phases:
                            for i, phase in enumerate(phases):
                                log(f"      Phase {i}:")
                                # Try to read phase details - phases are FSLFAiBossPhase structs
                                # Extract values from struct
                                try:
                                    # Access struct fields
                                    log(f"        (struct data present)")
                                except:
                                    pass
                    except Exception as e:
                        log(f"    Error reading phases: {e}")

                # Check combat managers
                if "Combat" in comp_class:
                    log(f"    >>> COMBAT MANAGER <<<")
                    try:
                        status_effects = comp.get_editor_property("default_attack_status_effects")
                        log(f"    DefaultAttackStatusEffects: {len(status_effects) if status_effects else 0}")
                        if status_effects:
                            for key, value in status_effects.items():
                                log(f"      Key: {key}")
                                log(f"      Value: {value}")
                    except Exception as e:
                        log(f"    Error: {e}")

                # Check ChildActorComponents
                if comp_class == "ChildActorComponent":
                    log(f"    >>> CHILD ACTOR <<<")
                    try:
                        child_class = comp.get_editor_property("child_actor_class")
                        log(f"    ChildActorClass: {child_class}")
                    except Exception as e:
                        log(f"    Error: {e}")

        # Check parent class B_Soulslike_Boss
        log("")
        log("--- Parent B_Soulslike_Boss ---")
        parent_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss"
        parent_bp = unreal.EditorAssetLibrary.load_asset(parent_path)
        if parent_bp:
            parent_gen = parent_bp.generated_class()
            parent_cdo = unreal.get_default_object(parent_gen)

            parent_comps = parent_cdo.get_components_by_class(unreal.ActorComponent)
            for comp in parent_comps:
                comp_class = comp.get_class().get_name()
                if "Boss" in comp_class or "Combat" in comp_class or comp_class == "ChildActorComponent":
                    log(f"  {comp.get_name()} ({comp_class})")

                    if "Boss" in comp_class:
                        try:
                            phases = comp.get_editor_property("phases")
                            log(f"    Phases: {len(phases) if phases else 0}")
                        except Exception as e:
                            log(f"    Error: {e}")

                    if comp_class == "ChildActorComponent":
                        try:
                            child_class = comp.get_editor_property("child_actor_class")
                            log(f"    ChildActorClass: {child_class}")
                        except:
                            pass

        # Check B_Soulslike_Enemy for default status effects
        log("")
        log("--- B_Soulslike_Enemy Status Effects ---")
        enemy_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy"
        enemy_bp = unreal.EditorAssetLibrary.load_asset(enemy_path)
        if enemy_bp:
            enemy_gen = enemy_bp.generated_class()
            enemy_cdo = unreal.get_default_object(enemy_gen)

            enemy_comps = enemy_cdo.get_components_by_class(unreal.ActorComponent)
            for comp in enemy_comps:
                comp_class = comp.get_class().get_name()
                if "Combat" in comp_class:
                    log(f"  {comp.get_name()} ({comp_class})")
                    try:
                        status_effects = comp.get_editor_property("default_attack_status_effects")
                        if status_effects:
                            log(f"    DefaultAttackStatusEffects: {len(status_effects)}")
                            for key, value in status_effects.items():
                                log(f"      Key: {key}")
                                log(f"      Value: {value}")
                        else:
                            log("    DefaultAttackStatusEffects: EMPTY")
                    except Exception as e:
                        log(f"    Error: {e}")

        log("")
        log("=" * 60)
        log("CHECK COMPLETE")
        log("=" * 60)

    except Exception as e:
        log(f"EXCEPTION: {e}")
        import traceback
        log(traceback.format_exc())
    finally:
        write_output()

if __name__ == "__main__":
    check_boss_backup()
