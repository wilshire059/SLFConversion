# diagnose_bb_state_key.py
# Check the BB_Standard State key type and configuration

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/bb_state_diagnosis.txt"

def log(msg):
    print(msg)
    with open(OUTPUT_FILE, "a") as f:
        f.write(msg + "\n")

def diagnose():
    # Clear output file
    with open(OUTPUT_FILE, "w") as f:
        f.write("=== BB_Standard State Key Diagnosis ===\n\n")

    bb_path = "/Game/SoulslikeFramework/Blueprints/_AI/BB_Standard"
    bb = unreal.load_asset(bb_path)

    if not bb:
        log(f"ERROR: Could not load {bb_path}")
        return

    log(f"Loaded: {bb.get_name()}")
    log(f"Class: {bb.get_class().get_name()}")

    # Get keys
    try:
        keys = bb.get_editor_property("keys")
        log(f"\nFound {len(keys)} keys:")

        for key in keys:
            try:
                key_name = key.get_editor_property("entry_name")
                key_class = key.get_class().get_name()
                log(f"\n  Key: {key_name}")
                log(f"    Class: {key_class}")

                # List all properties of this key
                for attr in dir(key):
                    if not attr.startswith('_') and not attr.startswith('get_') and not attr.startswith('set_') and not attr.startswith('cast'):
                        try:
                            val = getattr(key, attr)
                            if not callable(val) and val is not None:
                                log(f"    {attr}: {val}")
                        except:
                            pass

                # If enum key, try to get enum details
                if "Enum" in key_class:
                    log("    [ENUM KEY DETAILS]")
                    try:
                        enum_type = key.get_editor_property("enum_type")
                        log(f"      enum_type: {enum_type}")
                        if enum_type:
                            # Get enum values
                            log(f"      enum name: {enum_type.get_name()}")
                            log(f"      enum class: {enum_type.get_class().get_name()}")
                    except Exception as e:
                        log(f"      Error getting enum_type: {e}")

                    try:
                        enum_name = key.get_editor_property("enum_name")
                        log(f"      enum_name: {enum_name}")
                    except Exception as e:
                        log(f"      Error getting enum_name: {e}")

            except Exception as e:
                log(f"  Error reading key: {e}")

    except Exception as e:
        log(f"Error getting keys: {e}")

    # Check the actual enum E_AI_States
    log("\n\n=== E_AI_States Enum Check ===")
    enum_path = "/Game/SoulslikeFramework/Enums/E_AI_States"
    try:
        enum_asset = unreal.load_asset(enum_path)
        if enum_asset:
            log(f"Loaded enum: {enum_asset.get_name()}")
            log(f"Class: {enum_asset.get_class().get_name()}")

            # Try to get enum values
            for attr in dir(enum_asset):
                if not attr.startswith('_'):
                    try:
                        val = getattr(enum_asset, attr)
                        if not callable(val):
                            log(f"  {attr}: {val}")
                    except:
                        pass
        else:
            log("Enum E_AI_States not found (may have been deleted)")
    except Exception as e:
        log(f"Error loading enum: {e}")

    # Check C++ enum
    log("\n\n=== C++ ESLFAIStates Check ===")
    try:
        # ESLFAIStates is defined in C++
        from unreal import ESLFAIStates
        log("ESLFAIStates enum values:")
        for attr in dir(ESLFAIStates):
            if not attr.startswith('_'):
                try:
                    val = getattr(ESLFAIStates, attr)
                    if not callable(val):
                        log(f"  {attr}: {val}")
                except:
                    pass
    except Exception as e:
        log(f"Could not import ESLFAIStates: {e}")

    log("\n\nDiagnosis complete!")

if __name__ == "__main__":
    diagnose()
