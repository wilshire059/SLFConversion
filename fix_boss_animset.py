# fix_boss_animset.py
# Fixes the boss's ReactionAnimset to use DA_ExampleBossReactionAnimset instead of enemy animset
#
# Run: UnrealEditor-Cmd.exe "C:/scripts/SLFConversion/SLFConversion.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/fix_boss_animset.py" -stdout -unattended

import unreal

BOSS_ANIMSET_PATH = "/Game/SoulslikeFramework/Data/_AnimationData/DA_ExampleBossReactionAnimset"

# Boss character Blueprints to update
BOSS_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth",
]

def fix_boss_animset():
    """Set ReactionAnimset on boss character's AICombatManagerComponent."""

    unreal.log_warning("=" * 70)
    unreal.log_warning("FIXING BOSS REACTION ANIMSET")
    unreal.log_warning("=" * 70)

    # Load the boss animset
    boss_animset = unreal.load_asset(BOSS_ANIMSET_PATH)
    if not boss_animset:
        unreal.log_error(f"Failed to load boss animset: {BOSS_ANIMSET_PATH}")
        return

    unreal.log_warning(f"Loaded boss animset: {boss_animset.get_name()}")

    for bp_path in BOSS_BLUEPRINTS:
        unreal.log_warning(f"\n[{bp_path.split('/')[-1]}]")

        # Load the Blueprint
        bp = unreal.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"  [SKIP] Could not load Blueprint")
            continue

        # Get the generated class
        gen_class = bp.generated_class()
        if not gen_class:
            unreal.log_warning(f"  [SKIP] No generated class")
            continue

        # Get the CDO (Class Default Object)
        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            unreal.log_warning(f"  [SKIP] No CDO")
            continue

        unreal.log_warning(f"  CDO: {cdo.get_name()}")

        # Find AICombatManagerComponent on the CDO
        # First, try to get it as a property
        try:
            # Get all components on the actor
            components = cdo.get_components_by_class(unreal.ActorComponent)
            unreal.log_warning(f"  Found {len(components)} components on CDO")

            for comp in components:
                comp_name = comp.get_name()
                comp_class = comp.get_class().get_name()
                unreal.log_warning(f"    - {comp_name} ({comp_class})")

                if "AICombatManager" in comp_class or "AICombatManager" in comp_name:
                    # Found it - set the ReactionAnimset
                    try:
                        comp.set_editor_property("reaction_animset", boss_animset)
                        unreal.log_warning(f"  [OK] Set ReactionAnimset to {boss_animset.get_name()}")

                        # Save the Blueprint
                        unreal.EditorAssetLibrary.save_asset(bp_path)
                        unreal.log_warning(f"  [SAVED] {bp_path}")
                    except Exception as e:
                        unreal.log_warning(f"  [ERROR] Failed to set property: {e}")
                    break
            else:
                unreal.log_warning(f"  [SKIP] No AICombatManagerComponent found on CDO")

                # Try getting from Simple Construction Script (SCS)
                # The Blueprint's SCS contains component templates
                try:
                    scs = bp.simple_construction_script
                    if scs:
                        nodes = scs.get_all_nodes()
                        unreal.log_warning(f"  Found {len(nodes)} SCS nodes")

                        for node in nodes:
                            if node.component_template:
                                node_name = node.component_template.get_name()
                                node_class = node.component_template.get_class().get_name()
                                unreal.log_warning(f"    SCS: {node_name} ({node_class})")

                                if "AICombatManager" in node_class or "AICombatManager" in node_name:
                                    try:
                                        node.component_template.set_editor_property("reaction_animset", boss_animset)
                                        unreal.log_warning(f"  [OK] Set ReactionAnimset on SCS template to {boss_animset.get_name()}")

                                        # Save
                                        unreal.EditorAssetLibrary.save_asset(bp_path)
                                        unreal.log_warning(f"  [SAVED] {bp_path}")
                                    except Exception as e:
                                        unreal.log_warning(f"  [ERROR] Failed to set SCS property: {e}")
                                    break
                except Exception as e:
                    unreal.log_warning(f"  [NOTE] Could not access SCS: {e}")

        except Exception as e:
            unreal.log_warning(f"  [ERROR] {e}")

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("Done!")

if __name__ == "__main__":
    fix_boss_animset()
