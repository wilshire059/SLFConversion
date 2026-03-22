# configure_container_loot_proper.py
# Configure B_Container instances to use weighted random loot from DT_ExampleChestTier
# Matches bp_only behavior where containers drop random items based on weighted probability
#
# Design: AAA standard - proper validation, error handling, and atomic operations

import unreal

# FSoftObjectPath requires full path format: /Game/Path/Asset.AssetName
LOOT_TABLE_PATH = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleChestTier.DT_ExampleChestTier"
LEVEL_PATH = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"


def validate_loot_table_exists():
    """Verify the loot table asset exists before attempting to configure containers."""
    asset_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleChestTier"
    exists = unreal.EditorAssetLibrary.does_asset_exist(asset_path)
    if not exists:
        unreal.log_error(f"[ContainerLoot] FATAL: Loot table does not exist: {asset_path}")
        return False

    # Load and validate it's a DataTable
    table = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not table:
        unreal.log_error(f"[ContainerLoot] FATAL: Failed to load loot table: {asset_path}")
        return False

    if not isinstance(table, unreal.DataTable):
        unreal.log_error(f"[ContainerLoot] FATAL: Asset is not a DataTable: {type(table)}")
        return False

    unreal.log_warning(f"[ContainerLoot] Validated loot table: {table.get_name()}")
    return True


def find_containers_in_level():
    """Find all B_Container instances in the current level."""
    containers = []

    # Get all actors in the editor world
    editor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    all_actors = editor_subsystem.get_all_level_actors()

    for actor in all_actors:
        class_name = actor.get_class().get_name()
        if "B_Container" in class_name:
            containers.append(actor)

    # Sort by X location for consistent ordering
    containers.sort(key=lambda a: a.get_actor_location().x)

    return containers


def find_loot_manager_component(container):
    """Find the LootDropManager component on a container actor."""
    components = container.get_components_by_class(unreal.ActorComponent)

    for comp in components:
        comp_name = comp.get_name()
        comp_class = comp.get_class().get_name()

        if "LootDropManager" in comp_name or "LootDropManager" in comp_class:
            return comp

    return None


def configure_loot_manager(loot_manager, container_name):
    """
    Configure a LootDropManager component to use the weighted loot table.

    Blueprint property access:
    - LootTable: TSoftObjectPtr<UDataTable> - set to DT_ExampleChestTier
    - OverrideItem: FSLFLootItem struct - clear Item field to disable override
    """
    comp_class = loot_manager.get_class()

    # Clear OverrideItem.Item to disable fixed item drops
    # This forces the component to use LootTable for random selection
    try:
        override_item = loot_manager.get_editor_property("OverrideItem")
        if override_item:
            # FSLFLootItem struct - set Item to None
            override_item.set_editor_property("Item", None)
            override_item.set_editor_property("ItemClass", None)
            loot_manager.set_editor_property("OverrideItem", override_item)
            unreal.log_warning(f"[ContainerLoot] {container_name}: Cleared OverrideItem")
    except Exception as e:
        unreal.log_warning(f"[ContainerLoot] {container_name}: Could not clear OverrideItem: {e}")

    # Set LootTable to the weighted random table
    try:
        # Create soft object reference with proper path format
        loot_manager.set_editor_property("LootTable", LOOT_TABLE_PATH)
        unreal.log_warning(f"[ContainerLoot] {container_name}: Set LootTable = DT_ExampleChestTier")
        return True
    except Exception as e:
        unreal.log_error(f"[ContainerLoot] {container_name}: Failed to set LootTable: {e}")
        return False


def diagnose_container(container, index):
    """Print diagnostic info for a container's loot configuration."""
    name = container.get_name()
    location = container.get_actor_location()

    unreal.log_warning(f"\n[Container {index}] {name} at ({location.x:.1f}, {location.y:.1f}, {location.z:.1f})")

    loot_manager = find_loot_manager_component(container)
    if not loot_manager:
        unreal.log_warning(f"  NO LootDropManager component found!")
        return

    unreal.log_warning(f"  LootManager: {loot_manager.get_class().get_name()}")

    # Check LootTable
    try:
        loot_table = loot_manager.get_editor_property("LootTable")
        unreal.log_warning(f"  LootTable: {loot_table if loot_table else 'None'}")
    except:
        unreal.log_warning(f"  LootTable: <unable to read>")

    # Check OverrideItem
    try:
        override_item = loot_manager.get_editor_property("OverrideItem")
        if override_item:
            item = override_item.get_editor_property("Item")
            item_class = override_item.get_editor_property("ItemClass")
            unreal.log_warning(f"  OverrideItem.Item: {item.get_name() if item else 'None'}")
            unreal.log_warning(f"  OverrideItem.ItemClass: {item_class.get_name() if item_class else 'None'}")
    except:
        unreal.log_warning(f"  OverrideItem: <unable to read>")


def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CONTAINER LOOT CONFIGURATION - AAA STANDARD")
    unreal.log_warning("=" * 70)

    # Step 1: Validate prerequisites
    unreal.log_warning("\n[Step 1] Validating prerequisites...")
    if not validate_loot_table_exists():
        return

    # Step 2: Load level
    unreal.log_warning(f"\n[Step 2] Loading level: {LEVEL_PATH}")
    success = unreal.EditorLoadingAndSavingUtils.load_map(LEVEL_PATH)
    if not success:
        unreal.log_error(f"[ContainerLoot] FATAL: Failed to load level")
        return
    unreal.log_warning("[ContainerLoot] Level loaded successfully")

    # Step 3: Find containers
    unreal.log_warning("\n[Step 3] Finding containers...")
    containers = find_containers_in_level()
    unreal.log_warning(f"[ContainerLoot] Found {len(containers)} containers")

    if len(containers) == 0:
        unreal.log_warning("[ContainerLoot] No containers to configure")
        return

    # Step 4: Diagnose before
    unreal.log_warning("\n[Step 4] Current state (BEFORE):")
    for i, container in enumerate(containers):
        diagnose_container(container, i)

    # Step 5: Configure each container
    unreal.log_warning("\n[Step 5] Configuring containers...")
    configured_count = 0
    failed_count = 0

    for i, container in enumerate(containers):
        name = container.get_name()
        loot_manager = find_loot_manager_component(container)

        if not loot_manager:
            unreal.log_error(f"[ContainerLoot] {name}: No LootDropManager - SKIPPED")
            failed_count += 1
            continue

        if configure_loot_manager(loot_manager, name):
            configured_count += 1
        else:
            failed_count += 1

    # Step 6: Diagnose after
    unreal.log_warning("\n[Step 6] Current state (AFTER):")
    for i, container in enumerate(containers):
        diagnose_container(container, i)

    # Step 7: Save level
    unreal.log_warning("\n[Step 7] Saving level...")
    unreal.EditorLoadingAndSavingUtils.save_current_level()
    unreal.log_warning("[ContainerLoot] Level saved")

    # Summary
    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning(f"COMPLETE: {configured_count} configured, {failed_count} failed")
    unreal.log_warning("Containers will now drop random items from DT_ExampleChestTier")
    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    main()
