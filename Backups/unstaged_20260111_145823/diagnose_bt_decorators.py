"""
Diagnose Behavior Tree Decorators - dump full asset text to find IntValue
"""
import unreal
import re

def log(msg):
    unreal.log_warning(f"[BT_DEC] {msg}")

def main():
    log("=" * 70)
    log("BEHAVIOR TREE DECORATOR DIAGNOSTIC")
    log("=" * 70)

    # Export BT_Combat to text
    bt_combat_path = "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat"
    bt_combat = unreal.load_asset(bt_combat_path)

    if not bt_combat:
        log(f"[ERROR] Could not load: {bt_combat_path}")
        return

    log(f"Loaded: {bt_combat.get_name()}")

    # Get export text
    exporter = unreal.AssetExportTask()
    exporter.set_editor_property("object", bt_combat)
    exporter.set_editor_property("filename", "C:/scripts/SLFConversion/migration_cache/bt_combat_export.txt")
    exporter.set_editor_property("automated", True)
    exporter.set_editor_property("prompt", False)
    exporter.set_editor_property("exporter", None)

    success = unreal.Exporter.run_asset_export_task(exporter)
    if success:
        log("Exported BT_Combat to migration_cache/bt_combat_export.txt")
    else:
        log("[WARN] Export may have failed")

    # Also export BT_Idle
    bt_idle_path = "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Idle"
    bt_idle = unreal.load_asset(bt_idle_path)

    if bt_idle:
        exporter2 = unreal.AssetExportTask()
        exporter2.set_editor_property("object", bt_idle)
        exporter2.set_editor_property("filename", "C:/scripts/SLFConversion/migration_cache/bt_idle_export.txt")
        exporter2.set_editor_property("automated", True)
        exporter2.set_editor_property("prompt", False)

        success2 = unreal.Exporter.run_asset_export_task(exporter2)
        if success2:
            log("Exported BT_Idle to migration_cache/bt_idle_export.txt")

    # Export main BT_Enemy too
    bt_enemy_path = "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy"
    bt_enemy = unreal.load_asset(bt_enemy_path)

    if bt_enemy:
        exporter3 = unreal.AssetExportTask()
        exporter3.set_editor_property("object", bt_enemy)
        exporter3.set_editor_property("filename", "C:/scripts/SLFConversion/migration_cache/bt_enemy_export.txt")
        exporter3.set_editor_property("automated", True)
        exporter3.set_editor_property("prompt", False)

        success3 = unreal.Exporter.run_asset_export_task(exporter3)
        if success3:
            log("Exported BT_Enemy to migration_cache/bt_enemy_export.txt")

    log("")
    log("=== CHECK EXPORT FILES FOR IntValue ===")
    log("Look for 'IntValue' and 'BlackboardKey' in the export files")

if __name__ == "__main__":
    main()
