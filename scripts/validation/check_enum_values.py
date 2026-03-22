"""
Check E_AI_States enum values in the backup project
"""
import unreal

def log(msg):
    unreal.log_warning(f"[ENUM_CHECK] {msg}")

def main():
    log("=" * 70)
    log("CHECKING E_AI_STATES ENUM VALUES")
    log("=" * 70)

    # Try to load the Blueprint enum
    enum_path = "/Game/SoulslikeFramework/Enums/E_AI_States"
    enum_asset = unreal.load_asset(enum_path)

    if enum_asset:
        log(f"Found enum: {enum_asset.get_name()}")
        log(f"Class: {enum_asset.get_class().get_name()}")

        # Try to get enum values
        try:
            # UUserDefinedEnum doesn't expose values directly in Python
            # But we can try to get the display names
            names = enum_asset.get_editor_property("display_name_map")
            if names:
                log(f"Display names: {names}")
        except Exception as e:
            log(f"Error getting display names: {e}")

        # Try another approach - get all enum names
        try:
            num_enums = unreal.EnumBase.get_value_count(enum_asset)
            log(f"Value count: {num_enums}")
            for i in range(num_enums):
                name = unreal.EnumBase.get_name_by_index(enum_asset, i)
                display = unreal.EnumBase.get_display_name_by_index(enum_asset, i)
                log(f"  [{i}] Internal: {name}, Display: {display}")
        except Exception as e:
            log(f"Error enumerating: {e}")
    else:
        log(f"Could not load enum at {enum_path}")

    log("=" * 70)

if __name__ == "__main__":
    main()
