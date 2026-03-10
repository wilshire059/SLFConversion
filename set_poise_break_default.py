import unreal

def log(msg):
    print(f"[SetPoise] {msg}")
    unreal.log_warning(f"[SetPoise] {msg}")

log("=" * 70)
log("SET POISE BREAK ASSET DEFAULT")
log("=" * 70)

# Load the PoiseBreakAnimData Blueprint class
poise_path = "/Game/SoulslikeFramework/Data/_AnimationData/PDA_PoiseBreakAnimData"
poise_bp = unreal.EditorAssetLibrary.load_asset(poise_path)

if not poise_bp:
    log(f"ERROR: Could not load {poise_path}")
else:
    poise_class = poise_bp.generated_class()
    if not poise_class:
        log("ERROR: Could not get generated class from PoiseBreakAnimData")
    else:
        poise_cdo = unreal.get_default_object(poise_class)
        log(f"PoiseBreakAnimData CDO: {poise_cdo}")
        log(f"  Class: {poise_cdo.get_class().get_name()}")

        # Now set this on the boss's AC_AI_CombatManager component
        boss_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss"
        boss_bp = unreal.EditorAssetLibrary.load_asset(boss_path)

        if boss_bp:
            boss_class = boss_bp.generated_class()
            if boss_class:
                boss_cdo = unreal.get_default_object(boss_class)
                components = boss_cdo.get_components_by_class(unreal.ActorComponent)

                for comp in components:
                    comp_class = comp.get_class().get_name()
                    if "AC_AI_CombatManager" in comp_class:
                        log(f"Found AC_AI_CombatManager component: {comp.get_name()}")
                        try:
                            # Try to set PoiseBreakAsset on the component
                            comp.set_editor_property("poise_break_asset", poise_cdo)
                            log(f"  Set PoiseBreakAsset to: {poise_cdo}")

                            # Verify
                            verify = comp.get_editor_property("poise_break_asset")
                            log(f"  Verified: {verify}")
                        except Exception as e:
                            log(f"  Error setting property: {e}")

                # Save the boss Blueprint
                unreal.EditorAssetLibrary.save_asset(boss_path)
                log(f"Saved {boss_path}")
        else:
            log(f"ERROR: Could not load {boss_path}")

log("=" * 70)
