"""
Debug NPC interaction flow - check all required components and settings.
Run this on SLFConversion project to diagnose interaction issues.
"""
import unreal

def check_npc_interaction():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("NPC INTERACTION DEBUG - TRACING FULL FLOW")
    unreal.log_warning("="*70 + "\n")

    # ═══════════════════════════════════════════════════════════════════
    # STEP 1: Check B_Soulslike_NPC Blueprint parent class
    # ═══════════════════════════════════════════════════════════════════
    unreal.log_warning("═══ STEP 1: NPC Blueprint Parent Classes ═══")

    npc_paths = [
        "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor",
    ]

    for path in npc_paths:
        bp = unreal.EditorAssetLibrary.load_asset(path)
        if not bp:
            unreal.log_warning(f"  [MISSING] {path}")
            continue

        name = bp.get_name()
        gen_class = bp.generated_class()

        if gen_class:
            parent = gen_class.get_class().static_class()
            parent_name = "UNKNOWN"
            try:
                # Get parent class name
                parent_class = gen_class
                hierarchy = []
                while parent_class:
                    hierarchy.append(parent_class.get_name())
                    parent_class = parent_class.get_super_struct()
                parent_name = " -> ".join(hierarchy)
            except:
                pass

            unreal.log_warning(f"  {name}:")
            unreal.log_warning(f"    Generated: {gen_class.get_name()}")
            unreal.log_warning(f"    Hierarchy: {parent_name}")
        else:
            unreal.log_warning(f"  {name}: NO GENERATED CLASS!")

    # ═══════════════════════════════════════════════════════════════════
    # STEP 2: Check AC_AI_InteractionManager component
    # ═══════════════════════════════════════════════════════════════════
    unreal.log_warning("\n═══ STEP 2: AC_AI_InteractionManager Component ═══")

    comp_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_InteractionManager"
    comp_bp = unreal.EditorAssetLibrary.load_asset(comp_path)
    if comp_bp:
        gen_class = comp_bp.generated_class()
        if gen_class:
            unreal.log_warning(f"  Component Generated Class: {gen_class.get_name()}")

            # Get parent
            try:
                parent_class = gen_class.get_super_struct()
                unreal.log_warning(f"  Parent Class: {parent_class.get_name() if parent_class else 'NONE'}")
            except Exception as e:
                unreal.log_warning(f"  Parent Error: {e}")

            # Check CDO for properties
            try:
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    name_val = cdo.get_editor_property("name") if hasattr(cdo, "name") else "N/A"
                    unreal.log_warning(f"  CDO Name property: {name_val}")
            except Exception as e:
                unreal.log_warning(f"  CDO Error: {e}")
    else:
        unreal.log_warning(f"  [MISSING] {comp_path}")

    # ═══════════════════════════════════════════════════════════════════
    # STEP 3: Check PDA_Dialog and DA_ExampleDialog
    # ═══════════════════════════════════════════════════════════════════
    unreal.log_warning("\n═══ STEP 3: Dialog Data Assets ═══")

    dialog_paths = [
        "/Game/SoulslikeFramework/Data/Dialog/PDA_Dialog",
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog",
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DA_ExampleDialog_Vendor",
    ]

    for path in dialog_paths:
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if not asset:
            unreal.log_warning(f"  [MISSING] {path}")
            continue

        unreal.log_warning(f"  {asset.get_name()}:")
        unreal.log_warning(f"    Class: {asset.get_class().get_name()}")

        # Try to read properties
        try:
            # Check for DefaultDialogTable
            if hasattr(asset, "get_editor_property"):
                try:
                    default_table = asset.get_editor_property("default_dialog_table")
                    unreal.log_warning(f"    DefaultDialogTable: {default_table}")
                except:
                    pass

                try:
                    req = asset.get_editor_property("requirement")
                    unreal.log_warning(f"    Requirement: {req}")
                except:
                    pass
        except Exception as e:
            unreal.log_warning(f"    Property Error: {e}")

    # ═══════════════════════════════════════════════════════════════════
    # STEP 4: Check NPC instances in level for component configuration
    # ═══════════════════════════════════════════════════════════════════
    unreal.log_warning("\n═══ STEP 4: NPC Blueprint Instance Check (SCS) ═══")

    for npc_path in npc_paths:
        bp = unreal.EditorAssetLibrary.load_asset(npc_path)
        if not bp:
            continue

        name = bp.get_name()
        unreal.log_warning(f"\n  {name} Simple Construction Script:")

        # Try to access SCS
        try:
            scs = bp.simple_construction_script
            if scs:
                nodes = scs.get_all_nodes()
                for node in nodes:
                    comp_template = node.component_template
                    if comp_template:
                        comp_name = comp_template.get_name()
                        comp_class = comp_template.get_class().get_name()
                        unreal.log_warning(f"    - {comp_name} ({comp_class})")

                        # Check AC_AI_InteractionManager properties
                        if "InteractionManager" in comp_name or "InteractionManager" in comp_class:
                            try:
                                dialog_asset = comp_template.get_editor_property("dialog_asset")
                                npc_name = comp_template.get_editor_property("name")
                                vendor_asset = comp_template.get_editor_property("vendor_asset")
                                unreal.log_warning(f"      DialogAsset: {dialog_asset}")
                                unreal.log_warning(f"      Name: {npc_name}")
                                unreal.log_warning(f"      VendorAsset: {vendor_asset}")
                            except Exception as e:
                                unreal.log_warning(f"      Property read error: {e}")
        except Exception as e:
            unreal.log_warning(f"    SCS Error: {e}")

    # ═══════════════════════════════════════════════════════════════════
    # STEP 5: Check Interfaces
    # ═══════════════════════════════════════════════════════════════════
    unreal.log_warning("\n═══ STEP 5: Interface Implementation Check ═══")

    for npc_path in npc_paths:
        bp = unreal.EditorAssetLibrary.load_asset(npc_path)
        if not bp:
            continue

        name = bp.get_name()
        gen_class = bp.generated_class()
        if not gen_class:
            continue

        unreal.log_warning(f"  {name} Interfaces:")

        # Check for implemented interfaces
        try:
            # Get implemented interfaces from the Blueprint
            interfaces = unreal.BlueprintEditorLibrary.get_blueprint_implemented_interfaces(bp)
            for iface in interfaces:
                unreal.log_warning(f"    - {iface}")

            if len(interfaces) == 0:
                unreal.log_warning(f"    (NO INTERFACES)")
        except Exception as e:
            unreal.log_warning(f"    Interface Error: {e}")

    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("DEBUG COMPLETE")
    unreal.log_warning("="*70 + "\n")

if __name__ == "__main__":
    check_npc_interaction()
