"""Run PIE NPC Dialog Test - loads level and starts PIE with test command."""
import unreal
import subprocess
import os

def run_pie_npc_test():
    """Load demo level and check dialog setup."""

    unreal.log_warning("=" * 70)
    unreal.log_warning("[PIE NPC Test] Starting...")
    unreal.log_warning("=" * 70)

    # First, open the demo level
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    unreal.log_warning(f"[PIE NPC Test] Loading level: {level_path}")

    try:
        unreal.EditorLevelLibrary.load_level(level_path)
        unreal.log_warning("[PIE NPC Test] Level loaded")
    except Exception as e:
        unreal.log_error(f"[PIE NPC Test] Failed to load level: {e}")
        return

    # Get the current world
    world = unreal.EditorLevelLibrary.get_editor_world()
    unreal.log_warning(f"[PIE NPC Test] Current world: {world.get_name() if world else 'None'}")

    # Find all actors with AIInteractionManagerComponent (NPCs)
    all_actors = unreal.EditorLevelLibrary.get_all_level_actors()
    unreal.log_warning(f"[PIE NPC Test] Total actors in level: {len(all_actors)}")

    npc_count = 0
    for actor in all_actors:
        comps = actor.get_components_by_class(unreal.ActorComponent)
        for comp in comps:
            class_name = comp.get_class().get_name()
            if "AIInteractionManager" in class_name or "AI_InteractionManager" in class_name:
                npc_count += 1
                unreal.log_warning(f"\n[PIE NPC Test] === NPC FOUND: {actor.get_name()} ===")
                unreal.log_warning(f"  Component class: {class_name}")

                # Try to get dialog info
                try:
                    # Check for common dialog properties
                    for prop_name in ["Name", "name", "DialogAsset", "dialog_asset", "VendorAsset", "vendor_asset"]:
                        try:
                            val = comp.get_editor_property(prop_name)
                            if val is not None:
                                unreal.log_warning(f"  {prop_name}: {val}")
                        except:
                            pass
                except Exception as e:
                    unreal.log_warning(f"  Error getting properties: {e}")

    unreal.log_warning(f"\n[PIE NPC Test] Found {npc_count} NPCs with dialog components")

    # Check W_Dialog Blueprint reparenting
    unreal.log_warning("\n[PIE NPC Test] === Checking W_Dialog Blueprint ===")
    w_dialog_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Dialog"
    w_dialog_bp = unreal.EditorAssetLibrary.load_asset(w_dialog_path)
    if w_dialog_bp:
        unreal.log_warning(f"  W_Dialog Blueprint found")

        # Try to get parent class via Blueprint property
        try:
            parent_class = w_dialog_bp.get_editor_property("parent_class")
            if parent_class:
                parent_name = parent_class.get_name()
                parent_path = parent_class.get_path_name()
                unreal.log_warning(f"  W_Dialog parent class: {parent_name}")
                unreal.log_warning(f"  W_Dialog parent path: {parent_path}")

                if "SLFConversion" in parent_path and "W_Dialog" in parent_name:
                    unreal.log_warning("  [OK] W_Dialog is reparented to C++ UW_Dialog!")
                elif "UserWidget" in parent_name:
                    unreal.log_error("  [FAIL] W_Dialog still parented to UUserWidget - Cast will fail!")
                else:
                    unreal.log_warning(f"  [?] Unknown parent: {parent_name}")
            else:
                unreal.log_warning("  Could not get parent_class property")
        except Exception as e:
            unreal.log_warning(f"  Error getting parent class: {e}")

            # Fallback: try to load C++ class and check
            try:
                cpp_class = unreal.load_class(None, "/Script/SLFConversion.W_Dialog")
                if cpp_class:
                    unreal.log_warning(f"  C++ UW_Dialog class exists: {cpp_class.get_name()}")
                else:
                    unreal.log_error("  C++ UW_Dialog class NOT FOUND!")
            except Exception as e2:
                unreal.log_error(f"  Error loading C++ class: {e2}")
    else:
        unreal.log_error("[PIE NPC Test] W_Dialog Blueprint not found!")

    # Check DataTable structure
    unreal.log_warning("\n[PIE NPC Test] === Checking Dialog DataTables ===")
    dt_paths = [
        "/Game/SoulslikeFramework/Data/NPC/DT_DialogExampleGoodbye",
        "/Game/SoulslikeFramework/Data/NPC/DT_DialogExampleDefault",
    ]
    for dt_path in dt_paths:
        dt = unreal.EditorAssetLibrary.load_asset(dt_path)
        if dt:
            try:
                row_names = dt.get_row_names()
                row_struct = dt.get_editor_property("RowStruct")
                struct_path = row_struct.get_path_name() if row_struct else "None"
                struct_name = row_struct.get_name() if row_struct else "None"

                unreal.log_warning(f"\n  DataTable: {dt_path.split('/')[-1]}")
                unreal.log_warning(f"    RowStruct name: {struct_name}")
                unreal.log_warning(f"    RowStruct path: {struct_path}")
                unreal.log_warning(f"    Row count: {len(row_names)}")
                unreal.log_warning(f"    Row names: {row_names[:5]}")

                # Check if it's C++ or Blueprint struct
                if "/Script/SLFConversion" in struct_path:
                    unreal.log_warning("    [INFO] Uses C++ struct - direct FindRow will work")
                elif "/Game/" in struct_path:
                    unreal.log_warning("    [INFO] Uses Blueprint struct - needs reflection fallback")

            except Exception as e:
                unreal.log_warning(f"  Error reading DataTable: {e}")
        else:
            unreal.log_warning(f"  DataTable not found: {dt_path}")

    unreal.log_warning("\n" + "=" * 70)
    unreal.log_warning("[PIE NPC Test] Analysis complete.")
    unreal.log_warning("[PIE NPC Test] Now start PIE manually and run: SLF.Test.NPCDialog")
    unreal.log_warning("=" * 70)

# Run the test
run_pie_npc_test()
