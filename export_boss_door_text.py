"""Export B_BossDoor text to analyze SCS components"""
import unreal

BOSS_DOOR_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"
OUTPUT_PATH = "C:/scripts/SLFConversion/migration_cache/boss_door_text_bponly.txt"

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("EXPORTING B_BossDoor TEXT")
    unreal.log_warning("=" * 60)

    # Export the blueprint to text
    export_text = unreal.EditorAssetLibrary.export_text(BOSS_DOOR_PATH)
    if export_text:
        with open(OUTPUT_PATH, 'w', encoding='utf-8') as f:
            f.write(export_text)
        unreal.log_warning(f"Exported {len(export_text)} chars to {OUTPUT_PATH}")
    else:
        unreal.log_error("Failed to export")

if __name__ == "__main__":
    main()
