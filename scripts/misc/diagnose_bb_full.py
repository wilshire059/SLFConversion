# diagnose_bb_full.py
# Use SLFAutomationLibrary to diagnose BB_Standard State key

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/bb_full_diagnosis.txt"

def log(msg):
    print(msg)
    with open(OUTPUT_FILE, "a") as f:
        f.write(msg + "\n")

def diagnose():
    # Clear output file
    with open(OUTPUT_FILE, "w") as f:
        f.write("=== Full Blackboard Diagnosis ===\n\n")

    bb_path = "/Game/SoulslikeFramework/Blueprints/_AI/BB_Standard"

    # Use SLFAutomationLibrary
    try:
        result = unreal.SLFAutomationLibrary.diagnose_blackboard_state_key(bb_path)
        log("SLFAutomationLibrary.diagnose_blackboard_state_key result:")
        log(result)
    except Exception as e:
        log(f"Error calling diagnose_blackboard_state_key: {e}")

    # Also try compare_enum_values
    log("\n\n=== Enum Comparison ===")
    try:
        result = unreal.SLFAutomationLibrary.compare_enum_values()
        log(result)
    except Exception as e:
        log(f"Error calling compare_enum_values: {e}")

    # Try to export BB_Standard as text
    log("\n\n=== BB_Standard Text Export ===")
    bb = unreal.load_asset(bb_path)
    if bb:
        try:
            # Use export_text if available
            exported = unreal.EditorAssetLibrary.get_metadata_tag(bb_path, "State")
            log(f"Metadata tag 'State': {exported}")
        except Exception as e:
            log(f"Error getting metadata: {e}")

        # Try to find the State key entry
        log("\nIterating keys via editor property:")
        try:
            keys = bb.get_editor_property("keys")
            for i, key in enumerate(keys):
                log(f"  Key[{i}]: {type(key).__name__}")
                # Try to get properties from struct
                try:
                    entry_name = key.entry_name
                    log(f"    entry_name: {entry_name}")
                except:
                    pass
                try:
                    key_type = key.key_type
                    log(f"    key_type: {key_type}")
                    if key_type:
                        log(f"      key_type class: {type(key_type).__name__}")
                except:
                    pass
        except Exception as e:
            log(f"Error iterating keys: {e}")

    log("\nDiagnosis complete!")

if __name__ == "__main__":
    diagnose()
