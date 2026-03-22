"""
Export B_Soulslike_Character current state to check for BeginPlay override.
"""
import unreal

def log(msg):
    unreal.log_warning(msg)

def main():
    log("=" * 70)
    log("EXPORTING B_SOULSLIKE_CHARACTER STATE")
    log("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"

    bp = unreal.load_asset(bp_path)
    if not bp:
        log(f"[ERROR] Could not load {bp_path}")
        return

    try:
        state = unreal.SLFAutomationLibrary.export_blueprint_state(bp, "C:/scripts/SLFConversion/migration_cache/character_state.json")
        log(f"Exported to: {state}")
    except Exception as e:
        log(f"Error: {e}")

    log("=" * 70)

if __name__ == "__main__":
    main()
