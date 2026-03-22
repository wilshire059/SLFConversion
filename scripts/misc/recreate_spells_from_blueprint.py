# recreate_spells_from_blueprint.py
# Recreate spell data assets using the Blueprint PDA_Item class (PDA_Item_C)
# instead of direct C++ PDA_Item class

import unreal

LOG_FILE = "C:/scripts/SLFConversion/migration_cache/recreate_spells_log.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(str(msg))
    log_lines.append(str(msg))

# Template - use an existing working item as the base class
TEMPLATE_PATH = "/Game/SoulslikeFramework/Data/Items/DA_MagicSpell.DA_MagicSpell"

SPELLS = [
    {
        "name": "DA_FireballSpell",
        "display_name": "Fireball",
        "description": "Hurls a blazing fireball that explodes on impact, dealing fire damage.",
        "cpp_class": "/Script/SLFConversion.SLFFireballProjectile",
    },
    {
        "name": "DA_IceShardSpell",
        "display_name": "Ice Shard",
        "description": "Launches a crystalline ice shard that pierces enemies and slows them.",
        "cpp_class": "/Script/SLFConversion.SLFIceShardProjectile",
    },
    {
        "name": "DA_LightningBoltSpell",
        "display_name": "Lightning Bolt",
        "description": "Strikes enemies with a swift bolt of lightning. Very fast travel speed.",
        "cpp_class": "/Script/SLFConversion.SLFLightningBoltProjectile",
    },
    {
        "name": "DA_HolyOrbSpell",
        "display_name": "Holy Orb",
        "description": "Conjures a divine orb of radiant light. Effective against undead.",
        "cpp_class": "/Script/SLFConversion.SLFHolyOrbProjectile",
    },
    {
        "name": "DA_PoisonBlobSpell",
        "display_name": "Poison Blob",
        "description": "Throws a bubbling mass of toxic sludge that poisons enemies over time.",
        "cpp_class": "/Script/SLFConversion.SLFPoisonBlobProjectile",
    },
    {
        "name": "DA_ArcaneMissileSpell",
        "display_name": "Arcane Missile",
        "description": "Fires a magical projectile infused with pure arcane energy.",
        "cpp_class": "/Script/SLFConversion.SLFArcaneMissileProjectile",
    },
    {
        "name": "DA_BlackholeSpell",
        "display_name": "Blackhole",
        "description": "Conjures a dark void that pulls enemies in and deals dark damage.",
        "cpp_class": "/Script/SLFConversion.SLFSpellProjectile",
    },
    {
        "name": "DA_BeamSpell",
        "display_name": "Energy Beam",
        "description": "Channels a continuous beam of energy that damages all enemies in its path.",
        "cpp_class": "/Script/SLFConversion.SLFBeamSpell",
    },
    {
        "name": "DA_AOEExplosionSpell",
        "display_name": "Explosion",
        "description": "Creates a devastating explosion at your location, damaging all nearby enemies.",
        "cpp_class": "/Script/SLFConversion.SLFAOESpell",
    },
    {
        "name": "DA_ShieldSpell",
        "display_name": "Magic Shield",
        "description": "Creates a protective barrier around you that reduces incoming damage.",
        "cpp_class": "/Script/SLFConversion.SLFShieldSpell",
    },
    {
        "name": "DA_HealingSpell",
        "display_name": "Heal",
        "description": "Channels restorative energy to heal your wounds over time.",
        "cpp_class": "/Script/SLFConversion.SLFHealingSpell",
    },
    {
        "name": "DA_SummoningCircleSpell",
        "display_name": "Summoning Circle",
        "description": "Creates a magical circle of power that channels mystical energy.",
        "cpp_class": "/Script/SLFConversion.SLFSummoningCircle",
    },
]

SPELLS_FOLDER = "/Game/SoulslikeFramework/Data/Items/Spells"

def main():
    log("=" * 70)
    log("RECREATING SPELLS FROM BLUEPRINT CLASS")
    log("=" * 70)

    # Load template asset
    template = unreal.EditorAssetLibrary.load_asset(TEMPLATE_PATH)
    if not template:
        log(f"ERROR: Could not load template {TEMPLATE_PATH}")
        return

    template_class = template.get_class()
    log(f"Template class: {template_class.get_name()}")
    log(f"Template class path: {template_class.get_path_name()}")

    # Get the template's item_information to copy settings
    template_info = template.get_editor_property("item_information")
    template_category = template_info.get_editor_property("category")
    template_action = template_info.get_editor_property("action_to_trigger")

    log(f"Template category: {template_category.get_editor_property('category')}")
    log(f"Template action: {template_action}")

    success_count = 0

    for spell in SPELLS:
        spell_path = f"{SPELLS_FOLDER}/{spell['name']}.{spell['name']}"
        log(f"\nProcessing: {spell['name']}")

        # Delete existing asset if it exists
        if unreal.EditorAssetLibrary.does_asset_exist(spell_path):
            log(f"  Deleting existing asset...")
            unreal.EditorAssetLibrary.delete_asset(spell_path)

        # Duplicate from template
        new_path = f"{SPELLS_FOLDER}/{spell['name']}"
        result = unreal.EditorAssetLibrary.duplicate_asset(TEMPLATE_PATH, new_path)

        if not result:
            log(f"  ERROR: Failed to duplicate")
            continue

        # Load the duplicated asset
        asset = unreal.EditorAssetLibrary.load_asset(f"{new_path}.{spell['name']}")
        if not asset:
            log(f"  ERROR: Could not load duplicated asset")
            continue

        log(f"  Duplicated. Class: {asset.get_class().get_name()}")

        # Configure the spell
        item_info = asset.get_editor_property("item_information")

        # Set display name
        item_info.set_editor_property("display_name", spell['display_name'])

        # Set description
        item_info.set_editor_property("short_description", spell['description'])

        # Set ItemClass to the C++ spell class
        spell_class = unreal.load_class(None, spell['cpp_class'])
        if spell_class:
            item_info.set_editor_property("item_class", spell_class)
            log(f"  Set ItemClass: {spell['cpp_class']}")
        else:
            log(f"  WARNING: Could not load class {spell['cpp_class']}")

        # Keep the category and action_to_trigger from template
        # (they should already be copied via duplicate)

        # Save the item_information back
        asset.set_editor_property("item_information", item_info)

        # Save the asset
        if unreal.EditorAssetLibrary.save_asset(f"{new_path}.{spell['name']}", only_if_is_dirty=False):
            log(f"  SAVED: {spell['name']}")
            success_count += 1
        else:
            log(f"  WARNING: Save returned false")
            success_count += 1

    log(f"\n{'=' * 70}")
    log(f"COMPLETE: {success_count}/{len(SPELLS)} spells created")
    log("=" * 70)

    # Write log
    with open(LOG_FILE, 'w', encoding='utf-8') as f:
        f.write('\n'.join(log_lines))
    log(f"\nLog saved to: {LOG_FILE}")

if __name__ == "__main__":
    main()
