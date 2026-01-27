"""
Fix BB_Standard State key to use C++ ESLFAIStates enum instead of Blueprint E_AI_States
"""
import unreal

def log(msg):
    unreal.log_warning(f"[FIX_BB_ENUM] {msg}")

def main():
    log("=" * 70)
    log("FIXING BLACKBOARD STATE KEY ENUM TYPE")
    log("=" * 70)

    # Load the C++ enum
    cpp_enum_path = "/Script/SLFConversion.ESLFAIStates"
    cpp_enum = unreal.find_object(None, cpp_enum_path)

    if not cpp_enum:
        # Try loading via StaticClass pattern
        log(f"Direct find failed, trying alternate methods...")
        # The enum should be accessible as a UEnum
        try:
            cpp_enum = unreal.load_object(None, cpp_enum_path)
        except:
            pass

    if cpp_enum:
        log(f"Found C++ enum: {cpp_enum.get_path_name()}")
    else:
        log(f"[WARN] Could not find C++ enum at {cpp_enum_path}")
        log("Will try setting EnumName property instead...")

    # Load the blackboard
    bb_path = "/Game/SoulslikeFramework/Blueprints/_AI/BB_Standard"
    bb = unreal.load_asset(bb_path)

    if not bb:
        log(f"[ERROR] Could not load: {bb_path}")
        return

    log(f"Loaded: {bb.get_name()}")

    # Get the keys
    keys = bb.get_editor_property("keys")
    log(f"Keys count: {len(keys)}")

    state_key_found = False

    for i, key in enumerate(keys):
        key_name = key.get_editor_property("entry_name")

        if key_name == "State":
            state_key_found = True
            key_type = key.get_editor_property("key_type")

            if key_type:
                type_class = key_type.get_class().get_name()
                log(f"")
                log(f"Found State key at index {i}")
                log(f"  KeyType class: {type_class}")

                # Check current enum type
                try:
                    current_enum = key_type.get_editor_property("enum_type")
                    if current_enum:
                        log(f"  Current EnumType: {current_enum.get_path_name()}")
                    else:
                        log(f"  Current EnumType: None")
                except Exception as e:
                    log(f"  EnumType access error: {e}")

                # Check current enum name
                try:
                    current_name = key_type.get_editor_property("enum_name")
                    log(f"  Current EnumName: '{current_name}'")
                except Exception as e:
                    log(f"  EnumName access error: {e}")

                # Try to set the C++ enum
                log("")
                log("=== ATTEMPTING FIX ===")

                # Method 1: Set EnumType directly if we found the enum
                if cpp_enum:
                    try:
                        key_type.set_editor_property("enum_type", cpp_enum)
                        log(f"  [OK] Set enum_type to {cpp_enum.get_path_name()}")
                    except Exception as e:
                        log(f"  [FAIL] Could not set enum_type: {e}")

                # Method 2: Set EnumName to the C++ enum name
                try:
                    key_type.set_editor_property("enum_name", "ESLFAIStates")
                    log(f"  [OK] Set enum_name to 'ESLFAIStates'")
                except Exception as e:
                    log(f"  [FAIL] Could not set enum_name: {e}")

                # Method 3: Set bIsEnumNameValid
                try:
                    key_type.set_editor_property("is_enum_name_valid", True)
                    log(f"  [OK] Set is_enum_name_valid to True")
                except Exception as e:
                    log(f"  [FAIL] Could not set is_enum_name_valid: {e}")

                # Verify the changes
                log("")
                log("=== VERIFYING CHANGES ===")
                try:
                    new_enum = key_type.get_editor_property("enum_type")
                    if new_enum:
                        log(f"  New EnumType: {new_enum.get_path_name()}")
                    else:
                        log(f"  New EnumType: None")
                except:
                    pass

                try:
                    new_name = key_type.get_editor_property("enum_name")
                    log(f"  New EnumName: '{new_name}'")
                except:
                    pass

            break

    if not state_key_found:
        log("[ERROR] State key not found in blackboard!")
        return

    # Mark dirty and save
    log("")
    log("=== SAVING BLACKBOARD ===")
    try:
        bb.modify()
        saved = unreal.EditorAssetLibrary.save_asset(bb_path)
        if saved:
            log(f"[OK] Saved: {bb_path}")
        else:
            log(f"[WARN] save_asset returned False")
    except Exception as e:
        log(f"[ERROR] Save failed: {e}")

    log("")
    log("=" * 70)
    log("Done! Now re-save behavior trees to pick up the change.")
    log("=" * 70)

if __name__ == "__main__":
    main()
