# check_container_loot_table.py
# Check if B_Container's LootDropManager has LootTable set
# Run on bp_only to see original configuration

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECKING B_CONTAINER LOOT TABLE CONFIGURATION")
    unreal.log_warning("=" * 70)

    # Load B_Container Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load: {bp_path}")
        return

    unreal.log_warning(f"Blueprint: {bp.get_name()}")

    # Get CDO
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            unreal.log_warning(f"CDO: {cdo.get_name()}")

            # Get all components on CDO
            components = cdo.get_components_by_class(unreal.ActorComponent)
            for comp in components:
                comp_name = comp.get_name()
                comp_class = comp.get_class().get_name()
                unreal.log_warning(f"\nComponent: {comp_name} ({comp_class})")

                if "lootdropmanager" in comp_class.lower():
                    # Try to get properties
                    try:
                        loot_table = comp.get_editor_property("loot_table")
                        unreal.log_warning(f"  loot_table: {loot_table}")
                    except Exception as e:
                        unreal.log_warning(f"  loot_table error: {e}")

                    try:
                        override_item = comp.get_editor_property("override_item")
                        unreal.log_warning(f"  override_item: {override_item}")
                    except Exception as e:
                        unreal.log_warning(f"  override_item error: {e}")

    # Also check SCS for template values
    try:
        scs = bp.get_editor_property("simple_construction_script")
        if scs:
            nodes = scs.get_all_nodes()
            unreal.log_warning(f"\nSCS has {len(nodes)} nodes")

            for node in nodes:
                template = node.get_editor_property("component_template")
                if template:
                    comp_name = template.get_name()
                    comp_class = template.get_class().get_name()

                    if "lootdropmanager" in comp_class.lower():
                        unreal.log_warning(f"\nSCS LootDropManager template: {comp_name}")

                        # List all properties using dir()
                        for attr in dir(template):
                            if not attr.startswith('_') and not attr.startswith('get') and not attr.startswith('set'):
                                try:
                                    val = template.get_editor_property(attr)
                                    if val is not None:
                                        unreal.log_warning(f"  {attr}: {val}")
                                except:
                                    pass
    except Exception as e:
        unreal.log_warning(f"SCS error: {e}")

    unreal.log_warning("\n" + "=" * 70)

if __name__ == "__main__":
    main()
