"""Check B_BossDoor parent class in bp_only backup"""
import unreal

BOSS_DOOR_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("CHECKING B_BossDoor in bp_only")
    unreal.log_warning("=" * 60)

    bp = unreal.load_asset(BOSS_DOOR_PATH)
    if not bp:
        unreal.log_error(f"Could not load {BOSS_DOOR_PATH}")
        return

    unreal.log_warning(f"Blueprint: {bp.get_name()}")

    # Export to text and look for ParentClass
    export_text = unreal.EditorAssetLibrary.export_text(BOSS_DOOR_PATH)

    # Find ParentClass line
    for line in export_text.split('\n')[:50]:  # Check first 50 lines
        if 'ParentClass' in line or 'Parent' in line:
            unreal.log_warning(f"FOUND: {line.strip()}")
        if 'Actor' in line and 'Class' in line:
            unreal.log_warning(f"CLASS: {line.strip()}")

if __name__ == "__main__":
    main()
