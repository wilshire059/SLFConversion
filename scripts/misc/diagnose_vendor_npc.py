"""
Diagnose NPC Vendor configuration - mesh, animation, position
"""
import unreal

def diagnose_vendor_npc():
    print("=" * 60)
    print("DIAGNOSING NPC VENDOR CONFIGURATION")
    print("=" * 60)

    # Load the vendor Blueprint
    vendor_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor"
    vendor_bp = unreal.load_asset(vendor_path)

    if not vendor_bp:
        print(f"ERROR: Could not load vendor Blueprint at {vendor_path}")
        return

    print(f"\nVendor Blueprint: {vendor_bp.get_name()}")

    # Get generated class
    gen_class = vendor_bp.generated_class()
    print(f"Generated Class: {gen_class.get_name() if gen_class else 'NULL'}")

    if not gen_class:
        print("ERROR: No generated class!")
        return

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    print(f"CDO: {cdo.get_name()}")

    # Check mesh component from CDO
    print("\n=== MESH CONFIGURATION ===")
    try:
        mesh_comp = cdo.get_editor_property("mesh")
        if mesh_comp:
            print(f"Mesh Component: {mesh_comp.get_name()}")
            skel_mesh = mesh_comp.get_editor_property("skeletal_mesh")
            if skel_mesh:
                print(f"  Skeletal Mesh: {skel_mesh.get_name()}")
                print(f"  Mesh Path: {skel_mesh.get_path_name()}")
            else:
                print("  WARNING: No skeletal mesh set!")

            anim_class = mesh_comp.get_editor_property("anim_class")
            if anim_class:
                print(f"  AnimClass: {anim_class.get_name()}")
            else:
                print("  WARNING: No AnimClass set!")
        else:
            print("WARNING: No mesh component!")
    except Exception as e:
        print(f"Error accessing mesh: {e}")

    # Check C++ properties
    print("\n=== C++ PROPERTIES ===")
    try:
        default_vendor_mesh = cdo.get_editor_property("default_vendor_mesh")
        print(f"DefaultVendorMesh: {default_vendor_mesh}")

        npc_display_name = cdo.get_editor_property("npc_display_name")
        print(f"NPCDisplayName: {npc_display_name}")

        # Check NPCAnimClass from parent
        npc_anim_class = cdo.get_editor_property("npc_anim_class")
        print(f"NPCAnimClass: {npc_anim_class}")
    except Exception as e:
        print(f"Error accessing C++ properties: {e}")

    # Check AIInteractionManager
    print("\n=== AI INTERACTION MANAGER ===")
    try:
        ai_mgr = cdo.get_editor_property("ai_interaction_manager")
        if ai_mgr:
            print(f"AIInteractionManager: {ai_mgr.get_name()}")
            try:
                vendor_asset = ai_mgr.get_editor_property("vendor_asset")
                if vendor_asset:
                    print(f"  VendorAsset: {vendor_asset.get_name()}")
                else:
                    print("  WARNING: No VendorAsset!")
            except:
                pass

            try:
                dialog_asset = ai_mgr.get_editor_property("dialog_asset")
                if dialog_asset:
                    print(f"  DialogAsset: {dialog_asset.get_name()}")
            except:
                pass

            try:
                name = ai_mgr.get_editor_property("name")
                print(f"  Name: {name}")
            except:
                pass
        else:
            print("WARNING: No AIInteractionManager!")
    except Exception as e:
        print(f"Error accessing AIInteractionManager: {e}")

    # Also check the Guide for comparison
    print("\n" + "=" * 60)
    print("COMPARING WITH GUIDE NPC")
    print("=" * 60)

    guide_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide"
    guide_bp = unreal.load_asset(guide_path)

    if guide_bp:
        guide_class = guide_bp.generated_class()
        if guide_class:
            guide_cdo = unreal.get_default_object(guide_class)

            print(f"\nGuide CDO: {guide_cdo.get_name()}")

            try:
                mesh_comp = guide_cdo.get_editor_property("mesh")
                if mesh_comp:
                    skel_mesh = mesh_comp.get_editor_property("skeletal_mesh")
                    if skel_mesh:
                        print(f"Guide Mesh: {skel_mesh.get_name()}")
                    else:
                        print("Guide Mesh: NOT SET")
                    anim_class = mesh_comp.get_editor_property("anim_class")
                    if anim_class:
                        print(f"Guide AnimClass: {anim_class.get_name()}")
                    else:
                        print("Guide AnimClass: NOT SET")
            except Exception as e:
                print(f"Error accessing guide mesh: {e}")

    print("\n" + "=" * 60)
    print("DIAGNOSIS COMPLETE")
    print("=" * 60)

if __name__ == "__main__":
    diagnose_vendor_npc()
