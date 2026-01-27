# check_container_blueprint_loot.py
# Check B_Container Blueprint CDO for loot configuration

import unreal

def main():
    # Load B_Container Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container"
    bp = unreal.load_asset(bp_path)
    if not bp:
        unreal.log_error(f"Could not load: {bp_path}")
        return

    unreal.log_warning(f"\nBlueprint: {bp.get_name()}")

    # Check SCS for AC_LootDropManager component template
    try:
        scs = bp.get_editor_property("simple_construction_script")
        if scs:
            nodes = scs.get_all_nodes()
            for node in nodes:
                template = node.get_editor_property("component_template")
                if template:
                    comp_name = template.get_name()
                    comp_class = template.get_class().get_name()

                    if "lootdropmanager" in comp_class.lower():
                        unreal.log_warning(f"\nSCS Component: {comp_name} ({comp_class})")

                        # Try to export the component to see all properties
                        exported = template.export_text()
                        unreal.log_warning(f"\nExported text (first 2000 chars):")
                        unreal.log_warning(exported[:2000] if exported else "No export text")
    except Exception as e:
        unreal.log_warning(f"SCS error: {e}")

    # Also check AC_LootDropManager Blueprint directly
    loot_bp_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_LootDropManager"
    loot_bp = unreal.load_asset(loot_bp_path)
    if loot_bp:
        unreal.log_warning(f"\n\nAC_LootDropManager Blueprint found")

        # Try to get default values from CDO
        gen_class = loot_bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                exported = cdo.export_text()
                unreal.log_warning(f"\nCDO Exported text (first 3000 chars):")
                unreal.log_warning(exported[:3000] if exported else "No export text")

if __name__ == "__main__":
    main()
