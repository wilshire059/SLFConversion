"""
Apply reduced poison tick damage to DA_StatusEffect_Poison.

The Rank 3 poison values were previously set too high for testing:
- Instant HP: -250 (now -20 to -40)
- Tick HP: -25 (now -5 to -10)

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/apply_reduced_poison_damage.py" ^
  -stdout -unattended -nosplash 2>&1
"""

import unreal


def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("Applying Reduced Poison Damage Values")
    unreal.log_warning("=" * 70)

    poison_path = "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Poison"

    # Try to call the C++ function via different methods
    try:
        # Method 1: Direct static call (may work)
        result = unreal.SLFAutomationLibrary.apply_status_effect_rank_info(poison_path)
        unreal.log_warning(f"Result: {result}")
    except AttributeError as e:
        unreal.log_warning(f"Direct call failed: {e}")

        # Method 2: Try finding and calling via reflection
        try:
            # Get the function
            lib_class = unreal.find_class("SLFAutomationLibrary")
            if lib_class:
                unreal.log_warning(f"Found class: {lib_class}")
                # Try to call static function
                cdo = unreal.get_default_object(lib_class)
                if cdo:
                    result = cdo.apply_status_effect_rank_info(poison_path)
                    unreal.log_warning(f"Result: {result}")
        except Exception as e2:
            unreal.log_warning(f"CDO call failed: {e2}")

    # As a fallback, try applying all status effects
    try:
        unreal.log_warning("Trying to apply all status effect RankInfo...")
        result = unreal.SLFAutomationLibrary.apply_all_status_effect_rank_info()
        unreal.log_warning(f"ApplyAll Result: {result}")
    except AttributeError as e:
        unreal.log_warning(f"ApplyAll failed: {e}")

    unreal.log_warning("=" * 70)
    unreal.log_warning("Done!")
    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    main()
