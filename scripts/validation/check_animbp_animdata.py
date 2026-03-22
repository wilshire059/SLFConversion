#!/usr/bin/env python3
"""Check AnimDataAsset in AnimBP - Blueprint var vs C++ property"""

import unreal

def log(msg):
    print(f"[AnimDataCheck] {msg}")
    unreal.log_warning(f"[AnimDataCheck] {msg}")

def main():
    log("=" * 70)
    log("ANIMDATAASSET CHECK")
    log("=" * 70)

    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    log(f"Loaded: {bp.get_name()}")
    log(f"Class: {bp.get_class().get_name()}")

    # Get all Blueprint variables
    vars_list = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"\nBlueprint Variables ({len(vars_list)}):")
    for v in vars_list:
        log(f"  - {v}")

    # Check if AnimDataAsset is in Blueprint vars
    animdata_vars = [v for v in vars_list if 'AnimData' in v or 'anim_data' in v.lower()]
    if animdata_vars:
        log(f"\n[FOUND] AnimDataAsset Blueprint variables: {animdata_vars}")
        log("[WARNING] Blueprint has its own AnimDataAsset variable!")
        log("This may SHADOW the C++ property and be NULL!")
    else:
        log("\n[OK] No AnimDataAsset Blueprint variable found")

    # Get CDO and check property value
    log("\n--- CDO Property Check ---")
    gen_class_path = f"{animbp_path}.ABP_SoulslikeCharacter_Additive_C"
    gen_class = unreal.load_class(None, gen_class_path)

    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            # Check various property name formats
            prop_names = [
                'AnimDataAsset',
                'anim_data_asset',
                'AnimDataAsset_0',
            ]

            for prop in prop_names:
                try:
                    val = cdo.get_editor_property(prop)
                    log(f"  CDO.{prop} = {val.get_name() if val else 'NULL'}")
                    if val:
                        # Try to access sub-properties
                        try:
                            r = val.get_editor_property('two_handed_weapon_right')
                            l = val.get_editor_property('two_handed_weapon_left')
                            log(f"    TwoHandedWeapon_Right = {r.get_name() if r else 'NULL'}")
                            log(f"    TwoHandedWeapon_Left = {l.get_name() if l else 'NULL'}")
                        except Exception as e:
                            log(f"    Could not read TwoHanded props: {e}")
                except Exception as e:
                    log(f"  CDO.{prop} - not found or error: {e}")
    else:
        log("[ERROR] Could not load generated class")

    # Check parent class
    log("\n--- Parent Class ---")
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Parent: {parent}")

    # Check default value in Blueprint
    log("\n--- Blueprint Default Value ---")
    try:
        # Try to get AnimDataAsset default from Blueprint
        default_val = unreal.SLFAutomationLibrary.get_blueprint_variable_default(bp, "AnimDataAsset")
        log(f"Blueprint default AnimDataAsset = {default_val}")
    except Exception as e:
        log(f"Could not get Blueprint default: {e}")

    log("\n" + "=" * 70)

if __name__ == "__main__":
    main()
