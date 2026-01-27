"""
Diagnose blackboard enum key type - check what enum the State key references
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BB_ENUM] {msg}")

def main():
    log("=" * 70)
    log("BLACKBOARD ENUM TYPE DIAGNOSTIC")
    log("=" * 70)

    bb_path = "/Game/SoulslikeFramework/Blueprints/_AI/BB_Standard"
    bb = unreal.load_asset(bb_path)

    if not bb:
        log(f"[ERROR] Could not load: {bb_path}")
        return

    log(f"Loaded: {bb.get_name()}")

    # Get the keys
    keys = bb.get_editor_property("keys")
    log(f"Keys count: {len(keys)}")

    for i, key in enumerate(keys):
        key_name = key.get_editor_property("entry_name")
        key_type = key.get_editor_property("key_type")

        if key_type:
            type_class = key_type.get_class().get_name()
            log(f"")
            log(f"[{i}] {key_name}: {type_class}")

            # For enum types, get the enum class
            if "Enum" in type_class:
                try:
                    # Try to get enum type property
                    enum_type = key_type.get_editor_property("enum_type")
                    if enum_type:
                        log(f"    EnumType: {enum_type.get_path_name()}")
                    else:
                        log(f"    EnumType: None")
                except Exception as e:
                    log(f"    EnumType error: {e}")

                try:
                    # Try to get enum name
                    enum_name = key_type.get_editor_property("enum_name")
                    log(f"    EnumName: {enum_name}")
                except Exception as e:
                    log(f"    EnumName error: {e}")

                # Try export_text to see all properties
                try:
                    export = unreal.EditorAssetLibrary.export_text(key_type)
                    log(f"    Export: {export[:500] if export else 'None'}")
                except:
                    pass
        else:
            log(f"[{i}] {key_name}: No key type")

    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
