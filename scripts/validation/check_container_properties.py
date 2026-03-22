# check_container_properties.py
# List available properties on B_Container CDO

import unreal

BP_CONTAINER_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"

def check_container():
    """List all editable properties on B_Container CDO"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("CHECKING B_CONTAINER PROPERTIES")
    unreal.log_warning("=" * 60)

    # Load the Blueprint
    bp_asset = unreal.load_asset(BP_CONTAINER_PATH)
    if not bp_asset:
        unreal.log_error(f"Could not load Blueprint: {BP_CONTAINER_PATH}")
        return

    # Get generated class
    gen_class = bp_asset.generated_class()
    unreal.log_warning(f"Generated Class: {gen_class.get_name()}")

    # Parent class info would require different API

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log_error("Could not get CDO")
        return

    unreal.log_warning(f"\nCDO Type: {type(cdo).__name__}")

    # Try various property names (PascalCase as in Blueprint)
    properties_to_check = [
        "OpenMontage", "open_montage",
        "OpenVFX", "open_vfx",
        "MoveDistance", "move_distance",
        "SpeedMultiplier", "speed_multiplier",
        "CanBeTraced", "can_be_traced", "CanBeTraced?",
        "IsActivated", "is_activated", "IsActivated?",
        "InteractableDisplayName", "interactable_display_name",
        "InteractionText", "interaction_text",
        "CachedIntensity", "cached_intensity",
        "Lid", "lid",
        "ItemSpawn", "item_spawn",
        "PointLight", "point_light",
        "AC_LootDropManager", "ac_loot_drop_manager",
        "MoveToLocation", "move_to_location",
        "NiagaraLocation", "niagara_location",
    ]

    unreal.log_warning("\n--- CHECKING PROPERTIES ---")
    for prop in properties_to_check:
        try:
            val = cdo.get_editor_property(prop)
            unreal.log_warning(f"OK: {prop} = {val}")
        except Exception as e:
            unreal.log_warning(f"FAIL: {prop} - {str(e)[:60]}")

    unreal.log_warning("\n" + "=" * 60)
    unreal.log_warning("CHECK COMPLETE")
    unreal.log_warning("=" * 60)

if __name__ == "__main__":
    check_container()
