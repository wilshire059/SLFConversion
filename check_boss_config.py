"""
Check boss configuration in SLFConversion project.
This script examines the B_Soulslike_Boss_Malgareth Blueprint to diagnose:
1. AC_AI_Boss Phases array configuration
2. ChildActorComponent weapon configuration
3. AC_AI_CombatManager DefaultAttackStatusEffects configuration
"""

import unreal

# Output to a file for reliable capture
OUTPUT_FILE = "C:/scripts/SLFConversion/boss_config_report.txt"

output_lines = []

def log(msg):
    """Log a message to both unreal and our output file."""
    output_lines.append(str(msg))
    unreal.log_warning(str(msg))

def write_output():
    """Write all output lines to file."""
    with open(OUTPUT_FILE, 'w') as f:
        f.write('\n'.join(output_lines))

def check_boss_config():
    """Check the boss Blueprint configuration."""

    try:
        # Load the boss Blueprint
        boss_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth"
        boss_bp = unreal.EditorAssetLibrary.load_asset(boss_path)

        if not boss_bp:
            log(f"ERROR: Could not load boss Blueprint at {boss_path}")
            return

        log("")
        log("=" * 60)
        log(f"BOSS CONFIG CHECK: {boss_bp.get_name()}")
        log("=" * 60)

        # Get the generated class
        gen_class = boss_bp.generated_class()
        if not gen_class:
            log("ERROR: Could not get generated class")
            return

        # Get CDO
        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            log("ERROR: Could not get CDO")
            return

        log(f"CDO: {cdo}")
        log(f"Class: {gen_class.get_name()}")

        # Try to get components from CDO
        log("")
        log("--- Checking CDO Components ---")
        try:
            all_comps = cdo.get_components_by_class(unreal.ActorComponent)
            if all_comps:
                for comp in all_comps:
                    comp_class = comp.get_class().get_name()
                    log(f"  Component: {comp.get_name()} ({comp_class})")

                    # Check for AC_AI_Boss component
                    if "Boss" in comp_class or "AC_AI_Boss" in comp_class or comp_class == "AIBossComponent":
                        log(f"    FOUND BOSS COMPONENT!")
                        try:
                            phases = comp.get_editor_property("phases")
                            log(f"    Phases array: {len(phases) if phases else 0} entries")
                            if phases:
                                for i, phase in enumerate(phases):
                                    log(f"      Phase {i}: (struct data present)")
                            else:
                                log("    PROBLEM: Phases array is EMPTY!")
                        except Exception as e:
                            log(f"    Error reading phases: {e}")

                    # Check for AI Combat Manager
                    if "Combat" in comp_class and "Manager" in comp_class:
                        log(f"    FOUND COMBAT MANAGER!")
                        try:
                            status_effects = comp.get_editor_property("default_attack_status_effects")
                            log(f"    DefaultAttackStatusEffects: {len(status_effects) if status_effects else 0} entries")
                            if status_effects and len(status_effects) > 0:
                                for key in status_effects.keys():
                                    log(f"      - {key}")
                            else:
                                log("    PROBLEM: DefaultAttackStatusEffects is EMPTY!")
                        except Exception as e:
                            log(f"    Error reading status effects: {e}")

                    # Check for ChildActorComponent (weapons)
                    if comp_class == "ChildActorComponent":
                        try:
                            child_class = comp.get_editor_property("child_actor_class")
                            log(f"    ChildActorClass: {child_class if child_class else 'NOT SET!'}")
                        except Exception as e:
                            log(f"    Error getting ChildActorClass: {e}")
            else:
                log("  No components found on CDO")
        except Exception as e:
            log(f"  Error getting components: {e}")

        # Try to get components from SCS
        log("")
        log("--- Checking SCS (Blueprint Component Tree) ---")
        try:
            scs_nodes = unreal.BlueprintEditorLibrary.get_scs_nodes_from_blueprint(boss_bp)
            if scs_nodes:
                for node in scs_nodes:
                    template = node.component_template
                    if template:
                        comp_name = template.get_name()
                        comp_class = template.get_class().get_name()
                        log(f"  SCS: {comp_name} ({comp_class})")

                        # Check ChildActorComponents for weapons
                        if comp_class == "ChildActorComponent":
                            try:
                                child_class = template.get_editor_property("child_actor_class")
                                if child_class:
                                    log(f"    -> ChildActorClass: {child_class.get_name()}")
                                else:
                                    log(f"    -> ChildActorClass: NOT SET!")
                            except Exception as e:
                                log(f"    -> Error: {e}")

                        # Check for boss/combat components in SCS
                        if "Boss" in comp_class or "Combat" in comp_class:
                            log(f"    -> Found AI component in SCS: {comp_class}")
                            try:
                                phases = template.get_editor_property("phases")
                                log(f"    -> Phases: {len(phases) if phases else 0} entries")
                            except:
                                pass
            else:
                log("  No SCS nodes found")
        except Exception as e:
            log(f"  Error getting SCS: {e}")

        # Check parent Blueprint B_Soulslike_Boss
        log("")
        log("--- Checking Parent B_Soulslike_Boss ---")
        parent_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss"
        parent_bp = unreal.EditorAssetLibrary.load_asset(parent_path)
        if parent_bp:
            parent_gen = parent_bp.generated_class()
            parent_cdo = unreal.get_default_object(parent_gen)

            log(f"  Parent class: {parent_gen.get_name()}")

            # Check parent SCS for components
            try:
                parent_scs = unreal.BlueprintEditorLibrary.get_scs_nodes_from_blueprint(parent_bp)
                if parent_scs:
                    for node in parent_scs:
                        template = node.component_template
                        if template:
                            comp_class = template.get_class().get_name()
                            if "Boss" in comp_class or "Combat" in comp_class or comp_class == "ChildActorComponent":
                                log(f"  Parent SCS: {template.get_name()} ({comp_class})")

                                if comp_class == "ChildActorComponent":
                                    try:
                                        child_class = template.get_editor_property("child_actor_class")
                                        log(f"    -> ChildActorClass: {child_class.get_name() if child_class else 'NOT SET!'}")
                                    except:
                                        pass

                                if "Boss" in comp_class:
                                    try:
                                        phases = template.get_editor_property("phases")
                                        log(f"    -> Phases: {len(phases) if phases else 0} entries")
                                        if phases:
                                            for i, phase in enumerate(phases):
                                                log(f"      Phase {i} present")
                                    except Exception as e:
                                        log(f"    -> Error: {e}")
            except Exception as e:
                log(f"  Error: {e}")
        else:
            log("  Parent Blueprint not found")

        # Check B_Soulslike_Enemy for combat manager
        log("")
        log("--- Checking B_Soulslike_Enemy for DefaultAttackStatusEffects ---")
        enemy_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy"
        enemy_bp = unreal.EditorAssetLibrary.load_asset(enemy_path)
        if enemy_bp:
            enemy_gen = enemy_bp.generated_class()
            enemy_cdo = unreal.get_default_object(enemy_gen)

            try:
                enemy_comps = enemy_cdo.get_components_by_class(unreal.ActorComponent)
                for comp in enemy_comps:
                    comp_class = comp.get_class().get_name()
                    if "Combat" in comp_class:
                        log(f"  Enemy has: {comp.get_name()} ({comp_class})")
                        try:
                            status_effects = comp.get_editor_property("default_attack_status_effects")
                            if status_effects:
                                log(f"    DefaultAttackStatusEffects: {len(status_effects)} entries")
                                for key in status_effects.keys():
                                    log(f"      - {key}")
                            else:
                                log("    DefaultAttackStatusEffects: EMPTY")
                        except Exception as e:
                            log(f"    Error: {e}")
            except Exception as e:
                log(f"  Error: {e}")

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
        log(f"Output written to: {OUTPUT_FILE}")

if __name__ == "__main__":
    check_boss_config()
