# export_levelup_widget.py
# Export W_LevelUp widget structure and initialization logic from bp_only

import unreal
import json
import os

def export_widget_hierarchy(widget, depth=0):
    """Recursively export widget hierarchy"""
    result = {
        "name": widget.get_name(),
        "class": widget.get_class().get_name(),
        "children": []
    }

    # Try to get slot/panel children
    if hasattr(widget, 'get_all_children'):
        children = widget.get_all_children()
        for child in children:
            result["children"].append(export_widget_hierarchy(child, depth + 1))

    return result

def main():
    output = {
        "W_LevelUp": {},
        "W_StatBlock_LevelUp": {},
        "W_StatEntry_LevelUp": {},
        "W_StatEntry_StatName": {},
        "StatManager": {},
        "DT_ExampleStatTable": {}
    }

    unreal.log_warning("=" * 70)
    unreal.log_warning("EXPORTING LEVEL UP WIDGET DATA FROM BP_ONLY")
    unreal.log_warning("=" * 70)

    # =========================================================================
    # 1. Export W_LevelUp widget structure
    # =========================================================================
    unreal.log_warning("\n--- W_LevelUp Widget ---")
    levelup_path = "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelUp"
    levelup_bp = unreal.EditorAssetLibrary.load_asset(levelup_path)

    if levelup_bp:
        unreal.log_warning(f"Loaded: {levelup_bp.get_name()}")

        # Get widget tree
        try:
            widget_tree = levelup_bp.get_editor_property("widget_tree")
            if widget_tree:
                root = widget_tree.get_editor_property("root_widget")
                if root:
                    unreal.log_warning(f"Root widget: {root.get_name()} ({root.get_class().get_name()})")
        except Exception as e:
            unreal.log_warning(f"Could not get widget tree: {e}")

        # Export text for full details
        try:
            export_text = levelup_bp.export_text()

            # Find key patterns
            patterns = [
                "StatBlock", "StatEntry", "StatManager", "SetAllStatEntries",
                "InitializeStatEntries", "PopulateStats", "StatInfo", "Stat =",
                "BaseStats", "AttackStats", "DefenseStats", "NegationStats", "ResistanceStats",
                "AttributeStats", "EventGraph", "NativeConstruct", "PreConstruct"
            ]

            unreal.log_warning("\n--- Key patterns in W_LevelUp ---")
            lines = export_text.split('\n')
            for i, line in enumerate(lines):
                for pattern in patterns:
                    if pattern.lower() in line.lower():
                        unreal.log_warning(f"  [{i}] {line.strip()[:200]}")
                        break

            # Save full export
            output["W_LevelUp"]["export_lines"] = len(lines)

        except Exception as e:
            unreal.log_warning(f"Could not export text: {e}")

        # Check for variables
        unreal.log_warning("\n--- W_LevelUp Variables ---")
        try:
            gen_class = levelup_bp.generated_class()
            if gen_class:
                # Get CDO
                cdo = unreal.get_default_object(gen_class)

                # List all properties
                for prop in gen_class.get_editor_property("PropertyLink"):
                    unreal.log_warning(f"  Property: {prop}")

        except Exception as e:
            unreal.log_warning(f"Error getting variables: {e}")

    # =========================================================================
    # 2. Export W_StatBlock_LevelUp widget
    # =========================================================================
    unreal.log_warning("\n--- W_StatBlock_LevelUp Widget ---")
    statblock_path = "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_StatBlock_LevelUp"
    statblock_bp = unreal.EditorAssetLibrary.load_asset(statblock_path)

    if statblock_bp:
        unreal.log_warning(f"Loaded: {statblock_bp.get_name()}")

        try:
            export_text = statblock_bp.export_text()
            lines = export_text.split('\n')

            # Find stat-related patterns
            unreal.log_warning("\n--- Key patterns in W_StatBlock_LevelUp ---")
            patterns = ["StatEntry", "Stats", "Populate", "Initialize", "Children", "VerticalBox", "Add"]
            for i, line in enumerate(lines):
                for pattern in patterns:
                    if pattern.lower() in line.lower() and "stat" in line.lower():
                        unreal.log_warning(f"  [{i}] {line.strip()[:200]}")
                        break

            output["W_StatBlock_LevelUp"]["export_lines"] = len(lines)

        except Exception as e:
            unreal.log_warning(f"Could not export text: {e}")

    # =========================================================================
    # 3. Export W_StatEntry_LevelUp widget
    # =========================================================================
    unreal.log_warning("\n--- W_StatEntry_LevelUp Widget ---")
    statentry_path = "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_StatEntry_LevelUp"
    statentry_bp = unreal.EditorAssetLibrary.load_asset(statentry_path)

    if statentry_bp:
        unreal.log_warning(f"Loaded: {statentry_bp.get_name()}")

        try:
            export_text = statentry_bp.export_text()
            lines = export_text.split('\n')

            # Find key initialization patterns
            unreal.log_warning("\n--- Key patterns in W_StatEntry_LevelUp ---")
            patterns = ["Stat", "Initialize", "SetStat", "StatInfo", "Value", "Text", "Name"]
            for i, line in enumerate(lines):
                for pattern in patterns:
                    if pattern.lower() in line.lower():
                        unreal.log_warning(f"  [{i}] {line.strip()[:200]}")
                        break

            output["W_StatEntry_LevelUp"]["export_lines"] = len(lines)

        except Exception as e:
            unreal.log_warning(f"Could not export text: {e}")

    # =========================================================================
    # 4. Export Stat Table structure
    # =========================================================================
    unreal.log_warning("\n--- DT_ExampleStatTable ---")
    stattable_path = "/Game/SoulslikeFramework/Data/_Datatables/DT_ExampleStatTable"
    stattable = unreal.EditorAssetLibrary.load_asset(stattable_path)

    if stattable:
        unreal.log_warning(f"Loaded: {stattable.get_name()}")

        # Get row names
        row_names = stattable.get_row_names()
        unreal.log_warning(f"Row count: {len(row_names)}")

        stat_rows = []
        for row_name in row_names:
            unreal.log_warning(f"  Row: {row_name}")
            stat_rows.append(str(row_name))

        output["DT_ExampleStatTable"]["rows"] = stat_rows

    # =========================================================================
    # 5. Export AC_StatManager component
    # =========================================================================
    unreal.log_warning("\n--- AC_StatManager Component ---")
    statmgr_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager"
    statmgr_bp = unreal.EditorAssetLibrary.load_asset(statmgr_path)

    if statmgr_bp:
        unreal.log_warning(f"Loaded: {statmgr_bp.get_name()}")

        try:
            gen_class = statmgr_bp.generated_class()
            cdo = unreal.get_default_object(gen_class)

            # Check for stat table reference
            try:
                stat_table = cdo.get_editor_property("stat_table")
                unreal.log_warning(f"  StatTable: {stat_table}")
            except:
                pass

            try:
                stat_table = cdo.get_editor_property("StatTable")
                unreal.log_warning(f"  StatTable: {stat_table}")
            except:
                pass

        except Exception as e:
            unreal.log_warning(f"Error: {e}")

    # =========================================================================
    # 6. Check B_Stat class hierarchy
    # =========================================================================
    unreal.log_warning("\n--- B_Stat Classes ---")
    stat_paths = [
        "/Game/SoulslikeFramework/Blueprints/Stats/B_Stat",
        "/Game/SoulslikeFramework/Blueprints/Stats/B_HP",
        "/Game/SoulslikeFramework/Blueprints/Stats/B_Vigor",
        "/Game/SoulslikeFramework/Blueprints/Stats/B_Stamina",
    ]

    for stat_path in stat_paths:
        stat_bp = unreal.EditorAssetLibrary.load_asset(stat_path)
        if stat_bp:
            gen_class = stat_bp.generated_class()
            if gen_class:
                parent = gen_class.get_super_class()
                unreal.log_warning(f"  {stat_bp.get_name()} -> Parent: {parent.get_name() if parent else 'None'}")

                # Get CDO and check StatInfo
                cdo = unreal.get_default_object(gen_class)
                try:
                    stat_info = cdo.get_editor_property("stat_info")
                    unreal.log_warning(f"    StatInfo: {stat_info}")
                except:
                    pass
                try:
                    stat_info = cdo.get_editor_property("StatInfo")
                    unreal.log_warning(f"    StatInfo: {stat_info}")
                except:
                    pass

    # =========================================================================
    # 7. Save output
    # =========================================================================
    output_path = "C:/scripts/SLFConversion/migration_cache/levelup_widget_export.json"
    os.makedirs(os.path.dirname(output_path), exist_ok=True)

    with open(output_path, 'w') as f:
        json.dump(output, f, indent=2, default=str)

    unreal.log_warning(f"\n=== Saved to {output_path} ===")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
