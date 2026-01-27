"""
Check the blackboard State key enum type configuration
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BB_CHECK] {msg}")

def main():
    log("=" * 70)
    log("CHECKING BB_Standard BLACKBOARD ENUM CONFIG")
    log("=" * 70)

    bb_path = "/Game/SoulslikeFramework/Blueprints/_AI/BB_Standard"
    bb = unreal.load_asset(bb_path)

    if not bb:
        log(f"[ERROR] Could not load blackboard: {bb_path}")
        return

    log(f"Blackboard: {bb.get_name()}")
    log(f"Class: {bb.get_class().get_name()}")

    # Try to get keys
    keys = bb.get_editor_property('keys')
    log(f"Keys count: {len(keys) if keys else 0}")

    if keys:
        for i, key in enumerate(keys):
            key_name = key.get_editor_property('entry_name')
            key_type = key.get_editor_property('key_type')
            log(f"")
            log(f"[{i}] Key: {key_name}")
            log(f"    Type: {type(key_type).__name__ if key_type else 'None'}")

            if key_type:
                log(f"    Type class: {key_type.get_class().get_name()}")

                # Check if it's an enum key type
                if 'Enum' in key_type.get_class().get_name():
                    try:
                        enum_type = key_type.get_editor_property('enum_type')
                        log(f"    EnumType: {enum_type}")
                        if enum_type:
                            log(f"    EnumType name: {enum_type.get_name()}")
                            log(f"    EnumType path: {enum_type.get_path_name()}")
                    except Exception as e:
                        log(f"    EnumType error: {e}")

                    try:
                        enum_name = key_type.get_editor_property('enum_name')
                        log(f"    EnumName: {enum_name}")
                    except Exception as e:
                        log(f"    EnumName error: {e}")

    log("")
    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
