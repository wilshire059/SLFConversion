"""
Diagnose Blackboard - check key types and values
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BB_DIAG] {msg}")

def main():
    log("=" * 70)
    log("BLACKBOARD DIAGNOSTIC")
    log("=" * 70)

    # Load blackboard
    bb_path = "/Game/SoulslikeFramework/Blueprints/_AI/BB_Standard"
    bb = unreal.load_asset(bb_path)

    if not bb:
        log(f"[ERROR] Could not load blackboard: {bb_path}")
        return

    log(f"Loaded: {bb.get_name()}")
    log(f"Class: {bb.get_class().get_name()}")

    # List all properties to find the right one
    log("")
    log("Available properties:")
    for prop in dir(bb):
        if not prop.startswith('_'):
            try:
                val = getattr(bb, prop)
                if not callable(val):
                    log(f"  {prop}: {type(val).__name__}")
            except:
                pass

    # Try to get keys
    log("")
    log("Trying to get keys...")
    try:
        # UE5.7 might use different property name
        keys = bb.keys
        log(f"Found {len(keys)} keys")
        for i, key in enumerate(keys):
            log(f"  [{i}] {key}")
    except Exception as e:
        log(f"  Error accessing .keys: {e}")

    try:
        keys = bb.get_editor_property("Keys")
        log(f"Found {len(keys)} Keys")
        for i, key in enumerate(keys):
            try:
                name = key.get_editor_property("EntryName")
                key_type = key.get_editor_property("KeyType")
                log(f"  [{i}] {name}: {key_type}")
            except Exception as e2:
                log(f"  [{i}] Error: {e2}")
    except Exception as e:
        log(f"  Error accessing Keys: {e}")

if __name__ == "__main__":
    main()
