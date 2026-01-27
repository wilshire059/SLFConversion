#!/usr/bin/env python3
"""
Diagnose AnimBP variable state - check if Blueprint vars shadow C++ properties.
"""

import unreal

def log(msg):
    unreal.log_warning(f"[DIAGVARS] {msg}")

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
CHARACTER_PATH = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"

def diagnose_animbp_variables():
    """Check AnimBP variable state."""
    log("=== ANIMBP VARIABLES ===")

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        log("ERROR: Could not load AnimBP")
        return

    # Get generated class
    gen_class = bp.generated_class()
    if gen_class:
        log(f"Generated class: {gen_class.get_name()}")
        log(f"Parent class: {gen_class.get_super_class().get_name() if gen_class.get_super_class() else 'None'}")

    # Check for duplicate variables using C++ automation
    if hasattr(unreal, 'SLFAutomationLibrary'):
        log("")
        log("=== C++ DIAGNOSTIC ===")
        diag = unreal.SLFAutomationLibrary.diagnose_anim_bp(bp)

        # Look for variable section
        lines = diag.split('\n')
        in_var_section = False
        for line in lines:
            if 'VARIABLE' in line.upper() or 'OVERLAY' in line.lower():
                in_var_section = True
            if in_var_section:
                log(line)
                if line.strip() == '':
                    in_var_section = False

def check_runtime_values():
    """Spawn character and check AnimInstance values at runtime."""
    log("")
    log("=== RUNTIME VALUES ===")

    bp = unreal.EditorAssetLibrary.load_asset(CHARACTER_PATH)
    if not bp:
        log("ERROR: Could not load character BP")
        return

    gen_class = bp.generated_class()
    if not gen_class:
        log("No generated class")
        return

    # Spawn actor
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        log("No world")
        return

    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, unreal.Vector(0, 0, 30000))
    if not actor:
        log("Failed to spawn")
        return

    log(f"Spawned: {actor.get_name()}")

    # Get mesh component
    mesh_comps = actor.get_components_by_class(unreal.SkeletalMeshComponent)
    for mesh_comp in mesh_comps:
        if mesh_comp.get_name() == "CharacterMesh0":
            anim_instance = mesh_comp.get_anim_instance()
            if anim_instance:
                log(f"AnimInstance class: {anim_instance.get_class().get_name()}")
                log(f"AnimInstance parent: {anim_instance.get_class().get_super_class().get_name() if anim_instance.get_class().get_super_class() else 'None'}")

                # Check overlay properties
                props = [
                    "LeftHandOverlayState",
                    "RightHandOverlayState",
                    "ActiveOverlayState",
                    "left_hand_overlay_state",
                    "right_hand_overlay_state",
                    "active_overlay_state",
                ]

                log("")
                log("Overlay state values:")
                for prop in props:
                    try:
                        val = anim_instance.get_editor_property(prop)
                        log(f"  {prop}: {val}")
                    except Exception as e:
                        pass  # Skip non-existent variants

                # Check EquipmentManager
                try:
                    equip_mgr = actor.get_component_by_class(unreal.load_class(None, "/Script/SLFConversion.EquipmentManagerComponent"))
                    if equip_mgr:
                        log("")
                        log("EquipmentManager overlay states:")
                        try:
                            log(f"  LeftHandOverlayState: {equip_mgr.get_editor_property('LeftHandOverlayState')}")
                        except:
                            try:
                                log(f"  left_hand_overlay_state: {equip_mgr.get_editor_property('left_hand_overlay_state')}")
                            except:
                                pass
                        try:
                            log(f"  RightHandOverlayState: {equip_mgr.get_editor_property('RightHandOverlayState')}")
                        except:
                            try:
                                log(f"  right_hand_overlay_state: {equip_mgr.get_editor_property('right_hand_overlay_state')}")
                            except:
                                pass
                except Exception as e:
                    log(f"EquipmentManager not found: {e}")

    actor.destroy_actor()

def check_enum_values():
    """Check ESLFOverlayState enum values."""
    log("")
    log("=== ENUM VALUES ===")

    # Check C++ enum
    cpp_enum_path = "/Script/SLFConversion.ESLFOverlayState"
    try:
        cpp_enum = unreal.load_object(None, cpp_enum_path)
        if cpp_enum:
            log(f"C++ enum found: {cpp_enum.get_name()}")
    except:
        log("C++ enum: ESLFOverlayState (native, not loadable as object)")

    # Check Blueprint enum
    bp_enum_path = "/Game/SoulslikeFramework/Enums/E_OverlayState"
    bp_enum = unreal.EditorAssetLibrary.load_asset(bp_enum_path)
    if bp_enum:
        log(f"Blueprint enum found: {bp_enum.get_name()}")

        # Export enum values
        if hasattr(unreal, 'SLFAutomationLibrary'):
            values = unreal.SLFAutomationLibrary.export_blueprint_enum_values(bp_enum_path)
            log(f"Blueprint enum values: {values}")

def main():
    log("=" * 70)
    log("ANIMBP VARIABLE DIAGNOSTIC")
    log("=" * 70)

    diagnose_animbp_variables()
    check_runtime_values()
    check_enum_values()

    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
