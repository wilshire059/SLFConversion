# apply_equip_slots_cpp.py
# Apply EquipSlots tags using C++ automation library
# Run on SLFConversion project

import unreal

def log(msg):
    unreal.log_warning(str(msg))

# Configuration for equip slots
EQUIP_SLOTS_CONFIG = {
    "/Game/SoulslikeFramework/Data/Items/DA_Sword01": [
        "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1"
    ],
    "/Game/SoulslikeFramework/Data/Items/DA_Greatsword": [
        "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1"
    ],
    "/Game/SoulslikeFramework/Data/Items/DA_Katana": [
        "SoulslikeFramework.Equipment.SlotType.Right Hand Weapon 1"
    ],
    "/Game/SoulslikeFramework/Data/Items/DA_Shield01": [
        "SoulslikeFramework.Equipment.SlotType.Left Hand Weapon 1"
    ],
}

def apply_equip_slots():
    log("=" * 70)
    log("APPLYING EQUIP SLOTS VIA C++ AUTOMATION LIBRARY")
    log("=" * 70)

    # Get the automation library class
    try:
        automation_lib = unreal.SLFAutomationLibrary
        log("Found SLFAutomationLibrary")
    except AttributeError:
        log("ERROR: SLFAutomationLibrary not found!")
        log("Make sure the project is built with SLFAutomationLibrary exposed to Python")
        return

    success_count = 0

    for pda_path, slot_tags in EQUIP_SLOTS_CONFIG.items():
        pda_name = pda_path.split("/")[-1]
        log(f"\n{'='*60}")
        log(f"[{pda_name}]")
        log(f"  Tags to apply: {slot_tags}")

        # Call the C++ function
        try:
            result = automation_lib.apply_equip_slots_to_item(pda_path, slot_tags)
            if result:
                log(f"  SUCCESS: Applied {len(slot_tags)} tags")
                success_count += 1
            else:
                log(f"  FAILED: Could not apply tags")
        except Exception as e:
            log(f"  ERROR: {e}")

    log(f"\n\n{'='*70}")
    log(f"COMPLETE: {success_count}/{len(EQUIP_SLOTS_CONFIG)} items updated")
    log(f"{'='*70}")

if __name__ == "__main__":
    apply_equip_slots()
