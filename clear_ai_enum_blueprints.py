# clear_ai_enum_blueprints.py
# Clears EventGraph from AI Blueprints that reference E_AI_States
# These Blueprints have C++ parents that implement all logic

import unreal

def log(msg):
    unreal.log_warning(f"[CLEAR_AI] {msg}")

BLUEPRINTS_TO_CLEAR = [
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SwitchState",
    "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_SwitchToPreviousState",
    "/Game/SoulslikeFramework/Blueprints/_AI/Services/BTS_IsTargetDead",
    "/Game/SoulslikeFramework/Blueprints/_AI/Services/BTS_ChaseBounds",
    "/Game/SoulslikeFramework/AnimNotifies/AN_SetAiState",
]

def main():
    log("=" * 70)
    log("CLEARING AI BLUEPRINTS THAT REFERENCE E_AI_States")
    log("=" * 70)

    cleared_count = 0
    skip_count = 0
    error_count = 0

    for bp_path in BLUEPRINTS_TO_CLEAR:
        log(f"Processing: {bp_path}")

        bp = unreal.load_asset(bp_path)
        if not bp:
            log(f"  [ERROR] Could not load asset")
            error_count += 1
            continue

        # Try to clear EventGraph
        try:
            cleared = unreal.SLFAutomationLibrary.clear_eventgraph_only(bp)
            if cleared:
                # Compile and save
                unreal.SLFAutomationLibrary.compile_and_save(bp)
                log(f"  [OK] Cleared and saved")
                cleared_count += 1
            else:
                log(f"  [SKIP] No EventGraph to clear or already empty")
                skip_count += 1
        except Exception as e:
            log(f"  [ERROR] {str(e)}")
            error_count += 1

    log("")
    log("=" * 70)
    log(f"SUMMARY: {cleared_count} cleared, {skip_count} skipped, {error_count} errors")
    log("=" * 70)

if __name__ == "__main__":
    main()
