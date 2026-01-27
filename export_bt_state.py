"""
Export BT text and analyze State decorators
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BT_EXPORT] {msg}")

def main():
    log("=" * 70)
    log("EXPORTING BT_ENEMY STRUCTURE")
    log("=" * 70)

    # Export BT_Enemy
    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy"
    try:
        export = unreal.EditorAssetLibrary.export_text(bt_path)
        lines = export.split('\n')

        log(f"Total lines: {len(lines)}")
        log("")
        log("=== Looking for State decorator configuration ===")

        # Find State-related lines
        for i, line in enumerate(lines):
            if 'State' in line or 'IntValue' in line or 'BTDecorator' in line:
                if 'Blackboard' in line or 'State' in line or 'IntValue' in line:
                    log(f"  Line {i}: {line[:150]}")

        # Save full export
        with open("C:/scripts/SLFConversion/migration_cache/bt_enemy_export.txt", "w") as f:
            f.write(export)
        log(f"")
        log(f"Full export saved to: migration_cache/bt_enemy_export.txt")

    except Exception as e:
        log(f"Error: {e}")

    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
