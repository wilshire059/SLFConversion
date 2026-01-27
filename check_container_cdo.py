# check_container_cdo.py
# Compare B_Container CDO values between bp_only and current project

import unreal

# Paths
BP_CONTAINER_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container.B_Container_C"

def check_container():
    """Check B_Container CDO values"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("CHECKING B_CONTAINER CDO")
    unreal.log_warning("=" * 60)

    # Load B_Container class
    container_class = unreal.load_class(None, BP_CONTAINER_PATH)
    if not container_class:
        unreal.log_warning(f"ERROR: Could not load class: {BP_CONTAINER_PATH}")
        return

    # Get CDO
    cdo = unreal.get_default_object(container_class)
    if not cdo:
        unreal.log_warning("ERROR: Could not get CDO")
        return

    unreal.log_warning(f"\nClass: {container_class.get_name()}")
    unreal.log_warning(f"Parent: {unreal.SystemLibrary.get_class_display_name(container_class)}")

    # Check variables
    unreal.log_warning("\n--- VARIABLES ---")

    # OpenMontage
    try:
        open_montage = cdo.get_editor_property("open_montage")
        unreal.log_warning(f"OpenMontage: {open_montage}")
    except Exception as e:
        unreal.log_warning(f"OpenMontage: ERROR - {e}")

    # OpenVFX
    try:
        open_vfx = cdo.get_editor_property("open_vfx")
        unreal.log_warning(f"OpenVFX: {open_vfx}")
    except Exception as e:
        unreal.log_warning(f"OpenVFX: ERROR - {e}")

    # MoveDistance
    try:
        move_distance = cdo.get_editor_property("move_distance")
        unreal.log_warning(f"MoveDistance: {move_distance}")
    except Exception as e:
        unreal.log_warning(f"MoveDistance: ERROR - {e}")

    # SpeedMultiplier
    try:
        speed_mult = cdo.get_editor_property("speed_multiplier")
        unreal.log_warning(f"SpeedMultiplier: {speed_mult}")
    except Exception as e:
        unreal.log_warning(f"SpeedMultiplier: ERROR - {e}")

    # CachedIntensity
    try:
        cached_intensity = cdo.get_editor_property("cached_intensity")
        unreal.log_warning(f"CachedIntensity: {cached_intensity}")
    except Exception as e:
        unreal.log_warning(f"CachedIntensity: ERROR - {e}")

    # Check components
    unreal.log_warning("\n--- COMPONENTS ---")

    # Lid
    try:
        lid = cdo.get_editor_property("lid")
        unreal.log_warning(f"Lid: {lid}")
        if lid:
            try:
                mesh = lid.get_editor_property("static_mesh")
                unreal.log_warning(f"  StaticMesh: {mesh}")
            except:
                pass
    except Exception as e:
        unreal.log_warning(f"Lid: ERROR - {e}")

    # PointLight
    try:
        point_light = cdo.get_editor_property("point_light")
        unreal.log_warning(f"PointLight: {point_light}")
        if point_light:
            try:
                intensity = point_light.get_editor_property("intensity")
                unreal.log_warning(f"  Intensity: {intensity}")
            except:
                pass
    except Exception as e:
        unreal.log_warning(f"PointLight: ERROR - {e}")

    # AC_LootDropManager
    try:
        loot_mgr = cdo.get_editor_property("ac_loot_drop_manager")
        unreal.log_warning(f"AC_LootDropManager: {loot_mgr}")
        if loot_mgr:
            try:
                loot_table = loot_mgr.get_editor_property("loot_table")
                unreal.log_warning(f"  LootTable: {loot_table}")
            except:
                pass
            try:
                override_item = loot_mgr.get_editor_property("override_item")
                unreal.log_warning(f"  OverrideItem: {override_item}")
            except:
                pass
    except Exception as e:
        unreal.log_warning(f"AC_LootDropManager: ERROR - {e}")

    # ItemSpawn
    try:
        item_spawn = cdo.get_editor_property("item_spawn")
        unreal.log_warning(f"ItemSpawn: {item_spawn}")
    except Exception as e:
        unreal.log_warning(f"ItemSpawn: ERROR - {e}")

    # MoveToLocation
    try:
        move_to = cdo.get_editor_property("move_to_location")
        unreal.log_warning(f"MoveToLocation: {move_to}")
    except Exception as e:
        unreal.log_warning(f"MoveToLocation: ERROR - {e}")

    # NiagaraLocation
    try:
        niagara_loc = cdo.get_editor_property("niagara_location")
        unreal.log_warning(f"NiagaraLocation: {niagara_loc}")
    except Exception as e:
        unreal.log_warning(f"NiagaraLocation: ERROR - {e}")

    # Check inherited variables from B_Interactable
    unreal.log_warning("\n--- INHERITED (B_Interactable) ---")

    try:
        interactable_display_name = cdo.get_editor_property("interactable_display_name")
        unreal.log_warning(f"InteractableDisplayName: {interactable_display_name}")
    except Exception as e:
        unreal.log_warning(f"InteractableDisplayName: ERROR - {e}")

    try:
        interaction_text = cdo.get_editor_property("interaction_text")
        unreal.log_warning(f"InteractionText: {interaction_text}")
    except Exception as e:
        unreal.log_warning(f"InteractionText: ERROR - {e}")

    try:
        can_be_traced = cdo.get_editor_property("can_be_traced")
        unreal.log_warning(f"CanBeTraced: {can_be_traced}")
    except Exception as e:
        unreal.log_warning(f"CanBeTraced: ERROR - {e}")

    try:
        is_activated = cdo.get_editor_property("is_activated")
        unreal.log_warning(f"IsActivated: {is_activated}")
    except Exception as e:
        unreal.log_warning(f"IsActivated: ERROR - {e}")

    unreal.log_warning("\n" + "=" * 60)
    unreal.log_warning("CHECK COMPLETE")
    unreal.log_warning("=" * 60)

if __name__ == "__main__":
    check_container()
