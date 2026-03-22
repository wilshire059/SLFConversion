# run_migration_interfaces_only.py
# Phase 1: Reparent interfaces ONLY
# Run this first, then run the main migration script

import unreal

INTERFACE_MAP = {
    "BPI_Character": "/Script/SLFConversion.SLFCharacterInterface",
    "BPI_Controller": "/Script/SLFConversion.UBPI_Controller",
    "BPI_Interactable": "/Script/SLFConversion.SLFInteractableInterface",
    "BPI_NPC": "/Script/SLFConversion.SLFNPCInterface",
    "BPI_Projectile": "/Script/SLFConversion.SLFProjectileInterface",
    "BPI_SaveableActor": "/Script/SLFConversion.SLFSaveableActorInterface",
    "BPI_Weapon": "/Script/SLFConversion.SLFWeaponInterface",
}

INTERFACE_PATHS = [
    "/Game/SoulslikeFramework/Blueprints/Interfaces",
]

def find_interface(name):
    for path in INTERFACE_PATHS:
        full_path = path + "/" + name
        if unreal.EditorAssetLibrary.does_asset_exist(full_path):
            return unreal.EditorAssetLibrary.load_asset(full_path)
    return None

def run():
    print("=" * 60)
    print("INTERFACE MIGRATION - PHASE 1")
    print("=" * 60)

    success = 0
    failed = 0

    for name, cpp in INTERFACE_MAP.items():
        try:
            bp = find_interface(name)
            if bp:
                # Just save it - interfaces don't need clearing, they just need to exist
                # The C++ interface already has all the functions
                unreal.EditorAssetLibrary.save_loaded_asset(bp)
                print("  " + name + ": saved")
                success += 1
            else:
                print("  " + name + ": not found")
        except Exception as e:
            print("  " + name + ": error - " + str(e))
            failed += 1

    print("")
    print("Done: " + str(success) + " saved, " + str(failed) + " failed")

run()
