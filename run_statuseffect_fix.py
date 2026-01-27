# run_statuseffect_fix.py
# Run C++ automation to fix status effect RankInfo data (damage + VFX)

import unreal

unreal.log_warning("Running status effect RankInfo fix...")

# Call the C++ automation function
result = unreal.SLFAutomationLibrary.apply_all_status_effect_rank_info()
unreal.log_warning(f"Result: {result}")
