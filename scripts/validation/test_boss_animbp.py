import unreal

def log(msg):
    print(f"[BossAnimBP] {msg}")
    unreal.log_warning(f"[BossAnimBP] {msg}")

log("=" * 70)
log("BOSS ANIMBP COMPONENT VERIFICATION")
log("=" * 70)

# Load boss Blueprint
boss_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss"
boss_bp = unreal.EditorAssetLibrary.load_asset(boss_path)

if boss_bp:
    gen_class = boss_bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            # List components
            components = cdo.get_components_by_class(unreal.ActorComponent)
            log(f"Boss CDO has {len(components)} components")

            found_ac_ai_combat = False
            for comp in components:
                class_name = comp.get_class().get_name()
                comp_name = comp.get_name()
                if "AC_AI_CombatManager" in class_name or "AC_AI_CombatManager" in comp_name:
                    found_ac_ai_combat = True
                    log(f"  FOUND: {comp_name}: {class_name}")
                    # Try to access PoiseBreakAsset
                    try:
                        poise_break = comp.get_editor_property("poise_break_asset")
                        log(f"    PoiseBreakAsset: {poise_break}")
                    except Exception as e:
                        log(f"    PoiseBreakAsset access error: {e}")
                    try:
                        poise_broken = comp.get_editor_property("poise_broken")
                        log(f"    PoiseBroken: {poise_broken}")
                    except Exception as e:
                        log(f"    PoiseBroken access error: {e}")
                    try:
                        ik_weight = comp.get_editor_property("ik_weight")
                        log(f"    IkWeight: {ik_weight}")
                    except Exception as e:
                        log(f"    IkWeight access error: {e}")

            if found_ac_ai_combat:
                log("SUCCESS: AC_AI_CombatManager component found with accessible properties")
            else:
                log("ERROR: AC_AI_CombatManager component NOT found!")
                log("Available components:")
                for comp in components:
                    log(f"  - {comp.get_name()}: {comp.get_class().get_name()}")
else:
    log(f"ERROR: Could not load {boss_path}")

log("=" * 70)
