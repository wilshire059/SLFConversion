"""Compare Dialog Systems - Extract dialog data for comparison between projects."""
import unreal
import json
import os

def compare_dialog_systems():
    """Extract all dialog-related data for comparison."""

    results = {
        "project": "UNKNOWN",
        "npcs": [],
        "data_tables": [],
        "dialog_assets": [],
        "w_dialog_parent": None,
        "errors": []
    }

    # Determine which project we're in
    project_path = unreal.Paths.project_dir()
    if "SLFConversion" in project_path:
        results["project"] = "SLFConversion"
    elif "bp_only" in project_path:
        results["project"] = "bp_only"

    unreal.log_warning("=" * 70)
    unreal.log_warning(f"[Dialog Compare] Project: {results['project']}")
    unreal.log_warning("=" * 70)

    # 1. Load demo level
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    try:
        unreal.EditorLevelLibrary.load_level(level_path)
        unreal.log_warning(f"[Dialog Compare] Level loaded: {level_path}")
    except Exception as e:
        results["errors"].append(f"Failed to load level: {e}")
        unreal.log_error(f"[Dialog Compare] Failed to load level: {e}")

    # 2. Find NPCs and extract dialog info
    all_actors = unreal.EditorLevelLibrary.get_all_level_actors()
    unreal.log_warning(f"[Dialog Compare] Total actors: {len(all_actors)}")

    for actor in all_actors:
        comps = actor.get_components_by_class(unreal.ActorComponent)
        for comp in comps:
            class_name = comp.get_class().get_name()
            if "AIInteractionManager" in class_name or "AI_InteractionManager" in class_name:
                npc_data = {
                    "actor_name": actor.get_name(),
                    "component_class": class_name,
                    "npc_name": None,
                    "dialog_asset": None
                }

                try:
                    npc_data["npc_name"] = str(comp.get_editor_property("Name"))
                except:
                    pass

                try:
                    da = comp.get_editor_property("DialogAsset")
                    if da:
                        npc_data["dialog_asset"] = da.get_path_name()
                except:
                    pass

                results["npcs"].append(npc_data)
                unreal.log_warning(f"[Dialog Compare] NPC: {npc_data['actor_name']} -> {npc_data['dialog_asset']}")

    # 3. Check DataTables
    dt_paths = [
        "/Game/SoulslikeFramework/Data/Dialog/DT_GenericDefaultDialog",
    ]
    for dt_path in dt_paths:
        dt_data = {
            "path": dt_path,
            "exists": False,
            "row_struct": None,
            "row_struct_path": None,
            "rows": [],
            "sample_row_data": None
        }

        dt = unreal.EditorAssetLibrary.load_asset(dt_path)
        if dt:
            dt_data["exists"] = True
            try:
                row_struct = dt.get_editor_property("RowStruct")
                if row_struct:
                    dt_data["row_struct"] = row_struct.get_name()
                    dt_data["row_struct_path"] = row_struct.get_path_name()

                row_names = dt.get_row_names()
                dt_data["rows"] = [str(r) for r in row_names[:10]]

                unreal.log_warning(f"[Dialog Compare] DataTable: {dt_path}")
                unreal.log_warning(f"  RowStruct: {dt_data['row_struct']}")
                unreal.log_warning(f"  RowStruct path: {dt_data['row_struct_path']}")
                unreal.log_warning(f"  Rows: {dt_data['rows']}")

            except Exception as e:
                dt_data["error"] = str(e)
                results["errors"].append(f"DataTable {dt_path} error: {e}")

        results["data_tables"].append(dt_data)

    # 4. Check Dialog PDA Assets
    pda_paths = [
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog",
        "/Game/SoulslikeFramework/Data/Dialog/ShowcaseVendorNpc/DA_ExampleDialog_Vendor",
    ]
    for pda_path in pda_paths:
        pda_data = {
            "path": pda_path,
            "exists": False,
            "default_dialog_table": None
        }

        pda = unreal.EditorAssetLibrary.load_asset(pda_path)
        if pda:
            pda_data["exists"] = True
            try:
                ddt = pda.get_editor_property("DefaultDialogTable")
                if ddt:
                    # TSoftObjectPtr - get the path
                    pda_data["default_dialog_table"] = str(ddt)
                    unreal.log_warning(f"[Dialog Compare] PDA: {pda_path}")
                    unreal.log_warning(f"  DefaultDialogTable: {pda_data['default_dialog_table']}")
            except Exception as e:
                pda_data["error"] = str(e)

        results["dialog_assets"].append(pda_data)

    # 5. Check W_Dialog Blueprint parent
    w_dialog_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Dialog"
    w_dialog_bp = unreal.EditorAssetLibrary.load_asset(w_dialog_path)
    if w_dialog_bp:
        try:
            gen_class = w_dialog_bp.generated_class()
            if gen_class:
                results["w_dialog_parent"] = {
                    "generated_class": gen_class.get_name(),
                    "generated_class_path": gen_class.get_path_name()
                }
                unreal.log_warning(f"[Dialog Compare] W_Dialog generated class: {gen_class.get_name()}")
        except Exception as e:
            results["errors"].append(f"W_Dialog error: {e}")

    # 6. Check C++ class availability (only for SLFConversion)
    if results["project"] == "SLFConversion":
        try:
            cpp_class = unreal.load_class(None, "/Script/SLFConversion.W_Dialog")
            if cpp_class:
                results["cpp_w_dialog_class"] = {
                    "exists": True,
                    "name": cpp_class.get_name()
                }
                unreal.log_warning(f"[Dialog Compare] C++ UW_Dialog exists: {cpp_class.get_name()}")
        except:
            results["cpp_w_dialog_class"] = {"exists": False}

    # 7. Write results to file
    output_dir = project_path + "migration_cache/"
    os.makedirs(output_dir, exist_ok=True)
    output_file = output_dir + "dialog_comparison.json"

    with open(output_file, 'w') as f:
        json.dump(results, f, indent=2, default=str)

    unreal.log_warning(f"\n[Dialog Compare] Results written to: {output_file}")
    unreal.log_warning("=" * 70)

    return results

# Run the comparison
compare_dialog_systems()
