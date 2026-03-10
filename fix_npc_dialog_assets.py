"""Fix NPC Dialog Assets - Restore DialogAsset references that were lost during migration."""
import unreal

def fix_npc_dialog_assets():
    """Restore DialogAsset references on NPC Blueprints."""

    unreal.log_warning("=" * 70)
    unreal.log_warning("[Fix NPC Dialog] Starting...")
    unreal.log_warning("=" * 70)

    # Map of NPC Blueprints to their dialog assets
    npc_dialog_map = {
        "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide": "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor": "/Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DA_ExampleDialog_Vendor",
    }

    for npc_path, dialog_path in npc_dialog_map.items():
        unreal.log_warning(f"\n[Fix NPC Dialog] Processing: {npc_path.split('/')[-1]}")

        # Load NPC Blueprint
        npc_bp = unreal.EditorAssetLibrary.load_asset(npc_path)
        if not npc_bp:
            unreal.log_error(f"  [ERROR] Could not load NPC Blueprint: {npc_path}")
            continue

        # Load Dialog Asset
        dialog_asset = unreal.EditorAssetLibrary.load_asset(dialog_path)
        if not dialog_asset:
            unreal.log_error(f"  [ERROR] Could not load Dialog Asset: {dialog_path}")
            continue

        unreal.log_warning(f"  Dialog Asset: {dialog_asset.get_name()}")

        # Get the generated class and its CDO
        gen_class = npc_bp.generated_class()
        if not gen_class:
            unreal.log_error(f"  [ERROR] No generated class for NPC Blueprint")
            continue

        # Get CDO
        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            unreal.log_error(f"  [ERROR] Could not get CDO")
            continue

        # Find AIInteractionManagerComponent on CDO
        found_comp = None
        comps = cdo.get_components_by_class(unreal.ActorComponent)
        for comp in comps:
            class_name = comp.get_class().get_name()
            if "AIInteractionManager" in class_name or "AI_InteractionManager" in class_name:
                found_comp = comp
                unreal.log_warning(f"  Found component: {class_name}")
                break

        if not found_comp:
            unreal.log_warning(f"  [WARNING] No AIInteractionManager component found on CDO, checking SCS...")

            # Try to find in SimpleConstructionScript
            try:
                # Get all components in the Blueprint
                scs = npc_bp.simple_construction_script
                if scs:
                    nodes = scs.get_all_nodes()
                    for node in nodes:
                        template = node.get_editor_property("ComponentTemplate")
                        if template:
                            class_name = template.get_class().get_name()
                            if "AIInteractionManager" in class_name:
                                found_comp = template
                                unreal.log_warning(f"  Found component in SCS: {class_name}")
                                break
            except Exception as e:
                unreal.log_warning(f"  SCS access error: {e}")

        if found_comp:
            # Try to set DialogAsset
            try:
                # Check current value
                current = found_comp.get_editor_property("DialogAsset")
                unreal.log_warning(f"  Current DialogAsset: {current.get_name() if current else 'None'}")

                if current is None:
                    found_comp.set_editor_property("DialogAsset", dialog_asset)
                    unreal.log_warning(f"  [OK] Set DialogAsset to: {dialog_asset.get_name()}")

                    # Save the Blueprint
                    unreal.EditorAssetLibrary.save_asset(npc_path)
                    unreal.log_warning(f"  [OK] Saved Blueprint")
                else:
                    unreal.log_warning(f"  [INFO] DialogAsset already set")

            except Exception as e:
                unreal.log_error(f"  [ERROR] Failed to set DialogAsset: {e}")
        else:
            unreal.log_error(f"  [ERROR] Could not find AIInteractionManager component")

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("[Fix NPC Dialog] Complete")
    unreal.log_warning("=" * 70)

# Run the fix
fix_npc_dialog_assets()
