import unreal
import os
import sys

# Run this on bp_only project to check AC_AI_CombatManager defaults

def log(msg):
    print(f"[BP_Only] {msg}")
    unreal.log_warning(f"[BP_Only] {msg}")

log("=" * 70)
log("BP_ONLY AC_AI_COMBATMANAGER CHECK")
log("=" * 70)

# Check AC_AI_CombatManager Blueprint CDO
comp_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_CombatManager"
comp_bp = unreal.EditorAssetLibrary.load_asset(comp_path)

if comp_bp:
    log(f"AC_AI_CombatManager Blueprint loaded")
    gen_class = comp_bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            log(f"CDO class: {cdo.get_class().get_name()}")
            try:
                poise_break = cdo.get_editor_property("poise_break_asset")
                log(f"PoiseBreakAsset default: {poise_break}")
                if poise_break:
                    log(f"  Class: {poise_break.get_class().get_name()}")
            except Exception as e:
                log(f"PoiseBreakAsset error: {e}")
else:
    log(f"ERROR: Could not load AC_AI_CombatManager Blueprint")

# Also check boss Blueprint
boss_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss"
boss_bp = unreal.EditorAssetLibrary.load_asset(boss_path)

if boss_bp:
    log(f"Boss Blueprint loaded")
    gen_class = boss_bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            # Check components on boss
            components = cdo.get_components_by_class(unreal.ActorComponent)
            for comp in components:
                class_name = comp.get_class().get_name()
                if "CombatManager" in class_name or "Combat" in class_name:
                    log(f"Found combat component: {comp.get_name()}: {class_name}")
                    try:
                        poise_break = comp.get_editor_property("poise_break_asset")
                        log(f"  PoiseBreakAsset: {poise_break}")
                    except Exception as e:
                        log(f"  PoiseBreakAsset error: {e}")

log("=" * 70)
