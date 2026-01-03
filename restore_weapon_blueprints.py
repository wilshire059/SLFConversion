"""
Restore weapon Blueprints from backup to recover SCS component hierarchy.
The migration cleared the StaticMesh components from these Blueprints.
"""
import shutil
import os

# Source: backup with original Blueprint content
SRC_BASE = r"C:\scripts\SLFConversion_Migration\Backups\blueprint_only"

# Destination: current project
DST_BASE = r"C:\scripts\SLFConversion"

# Weapon-related Blueprints that need StaticMesh components restored
WEAPON_BLUEPRINTS = [
    # Base item and weapon
    "Content/SoulslikeFramework/Blueprints/_WorldActors/_Items/B_Item.uasset",
    "Content/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon.uasset",

    # AI Weapons (enemies)
    "Content/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_AI_Weapon.uasset",
    "Content/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_BossMace.uasset",
    "Content/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Greatsword.uasset",
    "Content/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword.uasset",

    # Player Weapons
    "Content/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_BossMace.uasset",
    "Content/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword.uasset",
    "Content/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana.uasset",
    "Content/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_PoisonSword.uasset",
    "Content/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield.uasset",
    "Content/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01.uasset",
    "Content/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample02.uasset",
]

def restore_files():
    print("=" * 70)
    print("RESTORING WEAPON BLUEPRINTS FROM BACKUP")
    print("=" * 70)
    print(f"Source: {SRC_BASE}")
    print(f"Destination: {DST_BASE}")
    print("")

    restored = 0
    not_found = 0

    for rel_path in WEAPON_BLUEPRINTS:
        src = os.path.join(SRC_BASE, rel_path)
        dst = os.path.join(DST_BASE, rel_path)
        name = os.path.basename(rel_path)

        if os.path.exists(src):
            os.makedirs(os.path.dirname(dst), exist_ok=True)
            shutil.copy2(src, dst)
            print(f"[OK] Restored: {name}")
            restored += 1
        else:
            print(f"[MISSING] Not in backup: {name}")
            not_found += 1

    print("")
    print("=" * 70)
    print(f"DONE: Restored {restored} files, {not_found} missing from backup")
    print("=" * 70)
    print("")
    print("NEXT STEPS:")
    print("1. Run the migration script with weapons EXCLUDED from clearing")
    print("2. OR add weapons to SKIP_LIST in run_migration.py")
    print("3. The restored Blueprints have their StaticMesh components intact")

if __name__ == "__main__":
    restore_files()
