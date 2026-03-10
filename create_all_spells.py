# create_all_spells.py
# Creates data assets for all spell types and adds them to player starting inventory

import unreal

LOG_FILE = "C:/scripts/SLFConversion/migration_cache/create_spells_log.txt"
log_lines = []

def log(msg):
    print(msg)
    unreal.log(msg)
    log_lines.append(str(msg))

# All spell configurations
SPELLS = [
    # Elemental Projectiles
    {
        "name": "DA_FireballSpell",
        "display_name": "Fireball",
        "description": "Hurls a blazing fireball that explodes on impact, dealing fire damage.",
        "cpp_class": "/Script/SLFConversion.SLFFireballProjectile",
        "category": "SoulslikeFramework.Item.Tool.Spell",
        "icon_color": (1.0, 0.4, 0.0),  # Orange
    },
    {
        "name": "DA_IceShardSpell",
        "display_name": "Ice Shard",
        "description": "Launches a crystalline ice shard that pierces enemies and slows them.",
        "cpp_class": "/Script/SLFConversion.SLFIceShardProjectile",
        "category": "SoulslikeFramework.Item.Tool.Spell",
        "icon_color": (0.3, 0.7, 1.0),  # Light blue
    },
    {
        "name": "DA_LightningBoltSpell",
        "display_name": "Lightning Bolt",
        "description": "Strikes enemies with a swift bolt of lightning. Very fast travel speed.",
        "cpp_class": "/Script/SLFConversion.SLFLightningBoltProjectile",
        "category": "SoulslikeFramework.Item.Tool.Spell",
        "icon_color": (1.0, 1.0, 0.3),  # Yellow
    },
    {
        "name": "DA_HolyOrbSpell",
        "display_name": "Holy Orb",
        "description": "Conjures a divine orb of radiant light. Effective against undead.",
        "cpp_class": "/Script/SLFConversion.SLFHolyOrbProjectile",
        "category": "SoulslikeFramework.Item.Tool.Spell",
        "icon_color": (1.0, 0.95, 0.7),  # Golden white
    },
    {
        "name": "DA_PoisonBlobSpell",
        "display_name": "Poison Blob",
        "description": "Throws a bubbling mass of toxic sludge that poisons enemies over time.",
        "cpp_class": "/Script/SLFConversion.SLFPoisonBlobProjectile",
        "category": "SoulslikeFramework.Item.Tool.Spell",
        "icon_color": (0.2, 0.9, 0.1),  # Toxic green
    },
    {
        "name": "DA_ArcaneMissileSpell",
        "display_name": "Arcane Missile",
        "description": "Fires a magical projectile infused with pure arcane energy.",
        "cpp_class": "/Script/SLFConversion.SLFArcaneMissileProjectile",
        "category": "SoulslikeFramework.Item.Tool.Spell",
        "icon_color": (0.6, 0.2, 1.0),  # Deep purple
    },
    {
        "name": "DA_BlackholeSpell",
        "display_name": "Blackhole",
        "description": "Conjures a dark void that pulls enemies in and deals dark damage.",
        "cpp_class": "/Script/SLFConversion.SLFSpellProjectile",
        "category": "SoulslikeFramework.Item.Tool.Spell",
        "icon_color": (0.1, 0.0, 0.2),  # Deep dark purple
    },
    # Special Effect Spells
    {
        "name": "DA_BeamSpell",
        "display_name": "Energy Beam",
        "description": "Channels a continuous beam of energy that damages all enemies in its path.",
        "cpp_class": "/Script/SLFConversion.SLFBeamSpell",
        "category": "SoulslikeFramework.Item.Tool.Spell",
        "icon_color": (0.2, 0.8, 1.0),  # Cyan
    },
    {
        "name": "DA_AOEExplosionSpell",
        "display_name": "Explosion",
        "description": "Creates a devastating explosion at your location, damaging all nearby enemies.",
        "cpp_class": "/Script/SLFConversion.SLFAOESpell",
        "category": "SoulslikeFramework.Item.Tool.Spell",
        "icon_color": (1.0, 0.5, 0.0),  # Orange explosion
    },
    {
        "name": "DA_ShieldSpell",
        "display_name": "Magic Shield",
        "description": "Creates a protective barrier around you that reduces incoming damage.",
        "cpp_class": "/Script/SLFConversion.SLFShieldSpell",
        "category": "SoulslikeFramework.Item.Tool.Spell",
        "icon_color": (0.3, 0.6, 1.0),  # Blue shield
    },
    {
        "name": "DA_HealingSpell",
        "display_name": "Heal",
        "description": "Channels restorative energy to heal your wounds over time.",
        "cpp_class": "/Script/SLFConversion.SLFHealingSpell",
        "category": "SoulslikeFramework.Item.Tool.Spell",
        "icon_color": (0.3, 1.0, 0.4),  # Soft green
    },
    {
        "name": "DA_SummoningCircleSpell",
        "display_name": "Summoning Circle",
        "description": "Creates a magical circle of power that channels mystical energy.",
        "cpp_class": "/Script/SLFConversion.SLFSummoningCircle",
        "category": "SoulslikeFramework.Item.Tool.Spell",
        "icon_color": (0.8, 0.2, 0.9),  # Magenta
    },
]

SPELLS_FOLDER = "/Game/SoulslikeFramework/Data/Items/Spells"
STARTING_INVENTORY_PATH = "/Game/SoulslikeFramework/Data/DT_StartingInventory"

def create_spell_data_assets():
    """Create data assets for all spells."""
    log("=" * 60)
    log("Creating Spell Data Assets")
    log("=" * 60)

    # Get or create the spells folder
    if not unreal.EditorAssetLibrary.does_directory_exist(SPELLS_FOLDER):
        unreal.EditorAssetLibrary.make_directory(SPELLS_FOLDER)
        log(f"Created folder: {SPELLS_FOLDER}")

    created_assets = []

    for spell in SPELLS:
        asset_path = f"{SPELLS_FOLDER}/{spell['name']}"
        log(f"\nCreating: {spell['name']}")

        try:
            # Check if asset already exists
            if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
                log(f"  Asset already exists, loading...")
                asset = unreal.EditorAssetLibrary.load_asset(asset_path)
            else:
                # Create new PDA_Item asset
                # Use the asset tools factory
                factory = unreal.DataAssetFactory()

                # Create the asset
                asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
                asset = asset_tools.create_asset(
                    spell['name'],
                    SPELLS_FOLDER,
                    unreal.PDA_Item,
                    factory
                )

                if not asset:
                    log(f"  ERROR: Failed to create asset")
                    continue

                log(f"  Created new asset")

            # Configure the asset
            # Set item information
            item_info = asset.get_editor_property("item_information")

            # Set display name (FSLFItemInfo uses DisplayName)
            item_info.set_editor_property("display_name", spell['display_name'])
            log(f"  Set name: {spell['display_name']}")

            # Set description (use short_description)
            item_info.set_editor_property("short_description", spell['description'])
            log(f"  Set description")

            # Set ItemClass to the C++ spell class
            spell_class = unreal.load_class(None, spell['cpp_class'])
            if spell_class:
                item_info.set_editor_property("item_class", spell_class)
                log(f"  Set ItemClass: {spell['cpp_class']}")
            else:
                log(f"  WARNING: Could not load class {spell['cpp_class']}")

            # Set category tag (Category is FSLFItemCategoryData struct)
            try:
                category_data = item_info.get_editor_property("category")
                category_tag = unreal.GameplayTag.request_gameplay_tag(spell['category'])
                category_data.set_editor_property("item_category", category_tag)
                item_info.set_editor_property("category", category_data)
                log(f"  Set category: {spell['category']}")
            except Exception as cat_error:
                log(f"  WARNING: Could not set category: {cat_error}")

            # Set item information back
            asset.set_editor_property("item_information", item_info)

            # Save the asset
            if unreal.EditorAssetLibrary.save_asset(asset_path, only_if_is_dirty=False):
                log(f"  SAVED: {asset_path}")
                created_assets.append(asset_path)
            else:
                log(f"  WARNING: Save returned false")
                created_assets.append(asset_path)  # Still count it

        except Exception as e:
            log(f"  ERROR: {e}")
            import traceback
            log(traceback.format_exc())

    log(f"\n{'=' * 60}")
    log(f"Created {len(created_assets)} spell assets")
    log("=" * 60)

    return created_assets


def add_spells_to_starting_inventory(spell_paths):
    """Add all spell data assets to the player's starting inventory."""
    log("\n" + "=" * 60)
    log("Adding Spells to Starting Inventory")
    log("=" * 60)

    # Load the starting inventory data table
    # Note: If it doesn't exist, we'll add to the player controller or game instance
    # For now, we'll use the existing approach from the magic spell

    # Try to find and modify PC_SoulslikeFramework's default inventory
    pc_path = "/Game/SoulslikeFramework/Blueprints/Global/PC_SoulslikeFramework"
    pc_bp = unreal.EditorAssetLibrary.load_asset(pc_path)

    if not pc_bp:
        log(f"ERROR: Could not load {pc_path}")
        return False

    log(f"Loaded: {pc_bp.get_name()}")

    # Get the generated class (CDO)
    gen_class = pc_bp.generated_class()
    if not gen_class:
        log("ERROR: Could not get generated class")
        return False

    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        log("ERROR: Could not get CDO")
        return False

    log(f"Got CDO: {cdo.get_name()}")

    # Get current starting inventory
    try:
        starting_inv = cdo.get_editor_property("starting_inventory")
        if starting_inv is None:
            starting_inv = []
        current_count = len(starting_inv)
        log(f"Current starting inventory has {current_count} items")
    except Exception as e:
        log(f"WARNING: Could not get starting_inventory: {e}")
        starting_inv = []

    # Add each spell to inventory
    added_count = 0
    for spell_path in spell_paths:
        spell_asset = unreal.EditorAssetLibrary.load_asset(spell_path)
        if not spell_asset:
            log(f"  Could not load: {spell_path}")
            continue

        # Check if already in inventory
        already_exists = False
        for item in starting_inv:
            if item and hasattr(item, 'item_asset'):
                existing_asset = item.item_asset
                if existing_asset and existing_asset.get_path_name() == spell_asset.get_path_name():
                    already_exists = True
                    break

        if already_exists:
            log(f"  Already in inventory: {spell_asset.get_name()}")
            continue

        # Create inventory item struct
        try:
            inv_item = unreal.SLFInventoryItem()
            inv_item.item_asset = spell_asset
            inv_item.amount = 1

            starting_inv.append(inv_item)
            log(f"  Added: {spell_asset.get_name()}")
            added_count += 1
        except Exception as e:
            log(f"  ERROR adding {spell_asset.get_name()}: {e}")

    # Save back to CDO
    try:
        cdo.set_editor_property("starting_inventory", starting_inv)
        log(f"\nSet starting_inventory with {len(starting_inv)} items")

        # Save the Blueprint
        if unreal.EditorAssetLibrary.save_asset(pc_path, only_if_is_dirty=False):
            log(f"SAVED: {pc_path}")
        else:
            log("WARNING: Save returned false")

    except Exception as e:
        log(f"ERROR setting starting_inventory: {e}")
        return False

    log(f"\n{'=' * 60}")
    log(f"Added {added_count} new spells to starting inventory")
    log("=" * 60)

    return True


def main():
    log("=" * 60)
    log("SPELL CREATION SCRIPT")
    log("=" * 60)

    # Step 1: Create all spell data assets
    created_paths = create_spell_data_assets()

    if not created_paths:
        log("\nNo spells were created!")
        return

    # Step 2: Add spells to starting inventory
    add_spells_to_starting_inventory(created_paths)

    log("\n" + "=" * 60)
    log("SPELL CREATION COMPLETE")
    log("=" * 60)
    log(f"\nCreated {len(SPELLS)} spell types:")
    for spell in SPELLS:
        log(f"  - {spell['display_name']}: {spell['description'][:50]}...")


if __name__ == "__main__":
    main()

    # Save log
    with open(LOG_FILE, 'w', encoding='utf-8') as f:
        f.write('\n'.join(log_lines))
    log(f"\nLog saved to: {LOG_FILE}")
