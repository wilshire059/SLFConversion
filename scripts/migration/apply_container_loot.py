# apply_container_loot.py
# Apply loot table to B_Container's AC_LootDropManager component

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("APPLYING LOOT TABLE TO B_Container")
    unreal.log_warning("=" * 70)

    # Load the B_Container Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load: {bp_path}")
        return

    # Load the loot table
    loot_table_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleChestTier"
    loot_table = unreal.load_asset(loot_table_path)
    if not loot_table:
        unreal.log_error(f"Could not load loot table: {loot_table_path}")
        return

    unreal.log_warning(f"Loot table loaded: {loot_table.get_name()}")

    # Get the Blueprint's SCS to find the AC_LootDropManager component template
    try:
        # Get SCS nodes
        scs_nodes = unreal.BlueprintEditorLibrary.get_scs_nodes(bp)
        unreal.log_warning(f"SCS nodes count: {len(scs_nodes) if scs_nodes else 0}")

        if scs_nodes:
            for node in scs_nodes:
                # Get the component template
                comp_template = node.get_editor_property("component_template")
                if comp_template:
                    comp_name = comp_template.get_name()
                    comp_class = comp_template.get_class().get_name()
                    unreal.log_warning(f"  SCS node: {comp_name} ({comp_class})")

                    if "lootdropmanager" in comp_class.lower():
                        unreal.log_warning(f"\n*** Found LootDropManager template ***")

                        # Try to set LootTable property
                        try:
                            comp_template.set_editor_property("loot_table", loot_table)
                            unreal.log_warning(f"Set loot_table on template!")
                        except Exception as e:
                            unreal.log_warning(f"Error setting loot_table: {e}")

                            # Try with soft reference
                            try:
                                soft_ref = unreal.SoftObjectPath(loot_table_path)
                                comp_template.set_editor_property("loot_table", soft_ref)
                                unreal.log_warning(f"Set loot_table as soft reference!")
                            except Exception as e2:
                                unreal.log_warning(f"Error setting soft ref: {e2}")

    except Exception as e:
        unreal.log_warning(f"SCS access error: {e}")

    # Also try setting on CDO's component
    unreal.log_warning("\n--- Trying CDO approach ---")
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            # Find LootDropManager component on CDO
            components = cdo.get_components_by_class(unreal.ActorComponent)
            for comp in components:
                comp_class = comp.get_class().get_name()
                if "lootdropmanager" in comp_class.lower():
                    unreal.log_warning(f"Found LootDropManager on CDO: {comp.get_name()}")
                    try:
                        comp.set_editor_property("loot_table", loot_table)
                        unreal.log_warning("Set loot_table on CDO component!")
                    except Exception as e:
                        unreal.log_warning(f"Error: {e}")

    # Save the Blueprint
    unreal.log_warning("\n--- Saving Blueprint ---")
    if unreal.EditorAssetLibrary.save_asset(bp_path):
        unreal.log_warning("Saved B_Container")
    else:
        unreal.log_warning("Failed to save")

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("Done")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
