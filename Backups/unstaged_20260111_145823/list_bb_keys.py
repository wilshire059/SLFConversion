"""
List all blackboard keys in BB_Standard to verify correct key names
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BB_KEYS] {msg}")

def main():
    log("=" * 70)
    log("LISTING ALL BLACKBOARD KEYS IN BB_STANDARD")
    log("=" * 70)

    bb_path = "/Game/SoulslikeFramework/Blueprints/_AI/BB_Standard"
    bb = unreal.load_asset(bb_path)

    if not bb:
        log("[ERROR] Could not load BB_Standard")
        return

    log(f"Loaded: {bb.get_name()}")
    log(f"Class: {bb.get_class().get_name()}")

    # Use C++ function to get keys
    result = unreal.SLFAutomationLibrary.diagnose_blackboard_keys(bb_path)
    if result:
        for line in result.split('\n'):
            log(line)
    else:
        log("[ERROR] No result from diagnose_blackboard_keys")

if __name__ == "__main__":
    main()
