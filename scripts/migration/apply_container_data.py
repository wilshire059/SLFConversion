# apply_container_data.py
# Apply B_Container CDO defaults to the Blueprint

import unreal

# Paths
BP_CONTAINER_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"

# Data to apply (from bp_only Blueprint)
CONTAINER_DATA = {
    # OpenMontage - Animation montage for opening
    "OpenMontage": "/Game/SoulslikeFramework/Demo/_Animations/Interaction/Container/AM_SLF_Container.AM_SLF_Container",
    # OpenVFX - Niagara system for chest glow (might not exist in this project)
    "OpenVFX": "/Game/SoulslikeFramework/Demo/_Niagara/NS_ItemLoot.NS_ItemLoot",
    # MoveDistance - How far lid moves (default is already 100 in C++)
    "MoveDistance": 100.0,
    # SpeedMultiplier - Animation speed
    "SpeedMultiplier": 1.0,
    # Inherited from B_Interactable
    "InteractableDisplayName": "Chest",
    "InteractionText": "Open",
    "CanBeTraced": True,
    "IsActivated": False,
}

def apply_container_data():
    """Apply container data to B_Container Blueprint CDO"""
    unreal.log_warning("=" * 60)
    unreal.log_warning("APPLYING CONTAINER DATA TO B_CONTAINER")
    unreal.log_warning("=" * 60)

    # Load the Blueprint asset
    bp_asset = unreal.load_asset(BP_CONTAINER_PATH)
    if not bp_asset:
        unreal.log_error(f"Could not load Blueprint: {BP_CONTAINER_PATH}")
        return False

    # Load the generated class
    gen_class = bp_asset.generated_class()
    if not gen_class:
        unreal.log_error(f"Could not load generated class")
        return False

    # Get CDO from generated class
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log_error("Could not get CDO")
        return False

    unreal.log_warning(f"Blueprint: {bp_asset.get_name()}")
    unreal.log_warning(f"CDO Type: {type(cdo).__name__}")

    success_count = 0
    fail_count = 0

    # Apply OpenMontage (TSoftObjectPtr needs the asset directly, not SoftObjectPath)
    try:
        montage_path = CONTAINER_DATA["OpenMontage"]
        montage = unreal.load_asset(montage_path)
        if montage:
            cdo.set_editor_property("OpenMontage", montage)
            unreal.log_warning(f"Set OpenMontage: {montage_path}")
            success_count += 1
        else:
            unreal.log_warning(f"Could not load montage: {montage_path}")
            fail_count += 1
    except Exception as e:
        unreal.log_warning(f"OpenMontage ERROR: {e}")
        fail_count += 1

    # Apply OpenVFX
    try:
        vfx_path = CONTAINER_DATA["OpenVFX"]
        vfx = unreal.load_asset(vfx_path)
        if vfx:
            soft_path = unreal.SoftObjectPath(vfx_path)
            cdo.set_editor_property("OpenVFX", soft_path)
            unreal.log_warning(f"Set OpenVFX: {vfx_path}")
            success_count += 1
        else:
            unreal.log_warning(f"Could not load VFX (asset may not exist): {vfx_path}")
            # Not counting as failure since asset may not exist in this project
    except Exception as e:
        unreal.log_warning(f"OpenVFX ERROR: {e}")

    # Apply MoveDistance
    try:
        cdo.set_editor_property("MoveDistance", CONTAINER_DATA["MoveDistance"])
        unreal.log_warning(f"Set MoveDistance: {CONTAINER_DATA['MoveDistance']}")
        success_count += 1
    except Exception as e:
        unreal.log_warning(f"MoveDistance ERROR: {e}")
        fail_count += 1

    # Apply SpeedMultiplier
    try:
        cdo.set_editor_property("SpeedMultiplier", CONTAINER_DATA["SpeedMultiplier"])
        unreal.log_warning(f"Set SpeedMultiplier: {CONTAINER_DATA['SpeedMultiplier']}")
        success_count += 1
    except Exception as e:
        unreal.log_warning(f"SpeedMultiplier ERROR: {e}")
        fail_count += 1

    # Apply inherited B_Interactable properties
    try:
        cdo.set_editor_property("InteractableDisplayName", unreal.Text(CONTAINER_DATA["InteractableDisplayName"]))
        unreal.log_warning(f"Set InteractableDisplayName: {CONTAINER_DATA['InteractableDisplayName']}")
        success_count += 1
    except Exception as e:
        unreal.log_warning(f"InteractableDisplayName ERROR: {e}")
        fail_count += 1

    try:
        cdo.set_editor_property("InteractionText", unreal.Text(CONTAINER_DATA["InteractionText"]))
        unreal.log_warning(f"Set InteractionText: {CONTAINER_DATA['InteractionText']}")
        success_count += 1
    except Exception as e:
        unreal.log_warning(f"InteractionText ERROR: {e}")
        fail_count += 1

    try:
        cdo.set_editor_property("CanBeTraced", CONTAINER_DATA["CanBeTraced"])
        unreal.log_warning(f"Set CanBeTraced: {CONTAINER_DATA['CanBeTraced']}")
        success_count += 1
    except Exception as e:
        unreal.log_warning(f"CanBeTraced ERROR: {e}")
        fail_count += 1

    try:
        cdo.set_editor_property("IsActivated", CONTAINER_DATA["IsActivated"])
        unreal.log_warning(f"Set IsActivated: {CONTAINER_DATA['IsActivated']}")
        success_count += 1
    except Exception as e:
        unreal.log_warning(f"IsActivated ERROR: {e}")
        fail_count += 1

    # Save the Blueprint
    if success_count > 0:
        saved = unreal.EditorAssetLibrary.save_asset(BP_CONTAINER_PATH, only_if_is_dirty=False)
        if saved:
            unreal.log_warning(f"Saved Blueprint: {BP_CONTAINER_PATH}")
        else:
            unreal.log_error(f"Failed to save Blueprint")

    unreal.log_warning(f"\nResults: {success_count} succeeded, {fail_count} failed")
    unreal.log_warning("=" * 60)
    unreal.log_warning("APPLY COMPLETE")
    unreal.log_warning("=" * 60)

    return success_count > 0

if __name__ == "__main__":
    apply_container_data()
