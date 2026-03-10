"""
Configure boss using C++ SLFAutomationLibrary functions.
"""
import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/boss_config_result.txt"

def configure_boss():
    # Asset paths
    boss_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth"
    cinematic_path = "/Game/SoulslikeFramework/Cinematics/LS_Boss_Start"
    weapon_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_BossMace"
    status_effect_path = "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Burn"

    # First diagnose the current state
    unreal.log_warning("=== DIAGNOSING BOSS BEFORE CONFIGURATION ===")
    diag_result = unreal.SLFAutomationLibrary.diagnose_boss_config(boss_path)
    unreal.log_warning(diag_result)

    # Configure the boss
    unreal.log_warning("=== CONFIGURING BOSS ===")
    config_result = unreal.SLFAutomationLibrary.configure_boss(
        boss_path,
        cinematic_path,
        weapon_path,
        status_effect_path,
        1,    # StatusEffectRank
        25.0  # StatusEffectBuildup
    )
    unreal.log_warning(config_result)

    # Diagnose after configuration
    unreal.log_warning("=== DIAGNOSING BOSS AFTER CONFIGURATION ===")
    diag_result2 = unreal.SLFAutomationLibrary.diagnose_boss_config(boss_path)
    unreal.log_warning(diag_result2)

    # Write results to file
    with open(OUTPUT_FILE, 'w') as f:
        f.write("=== BOSS CONFIGURATION RESULTS ===\n\n")
        f.write("--- BEFORE ---\n")
        f.write(diag_result + "\n\n")
        f.write("--- CONFIGURATION ---\n")
        f.write(config_result + "\n\n")
        f.write("--- AFTER ---\n")
        f.write(diag_result2 + "\n")

    unreal.log_warning(f"Results written to {OUTPUT_FILE}")

if __name__ == "__main__":
    configure_boss()
