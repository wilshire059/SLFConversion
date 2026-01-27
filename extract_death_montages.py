# extract_death_montages.py
# Extracts Death TMap data from PDA_CombatReactionAnimData assets
#
# Run on bp_only project:
# UnrealEditor-Cmd.exe "C:/scripts/bp_only/bp_only.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/extract_death_montages.py" -stdout -unattended

import unreal
import json
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/death_montages.json"

# Reaction animset paths to extract
REACTION_ANIMSETS = [
    "/Game/SoulslikeFramework/Data/_AnimationData/DA_ExampleEnemyReactionAnimset",
    "/Game/SoulslikeFramework/Data/_AnimationData/DA_ExampleBossReactionAnimset",
    "/Game/SoulslikeFramework/Data/_AnimationData/DA_ExamplePlayerReactionAnimset",
]

# E_Direction enum values mapping
DIRECTION_NAMES = {
    0: "Idle",
    1: "Fwd",
    2: "FwdLeft",
    3: "FwdRight",
    4: "Left",
    5: "Right",
    6: "Bwd",
    7: "BwdLeft",
    8: "BwdRight",
}

def extract_death_montages():
    """Extract Death TMap entries from reaction animset data assets."""

    unreal.log_warning("=" * 70)
    unreal.log_warning("EXTRACTING DEATH MONTAGES FROM REACTION ANIMSETS")
    unreal.log_warning("=" * 70)

    result = {}

    for asset_path in REACTION_ANIMSETS:
        asset_name = asset_path.split("/")[-1]
        unreal.log_warning(f"\n[{asset_name}]")

        # Load the asset
        asset = unreal.load_asset(asset_path)
        if not asset:
            unreal.log_warning(f"  [SKIP] Could not load asset")
            continue

        unreal.log_warning(f"  Class: {asset.get_class().get_name()}")

        asset_data = {
            "path": asset_path,
            "death_montages": {},
            "hit_reactions": {}
        }

        # Try to get Death property
        try:
            death_map = asset.get_editor_property("Death")
            if death_map:
                unreal.log_warning(f"  Death TMap found with {len(death_map)} entries")
                for direction, montage_ref in death_map.items():
                    # Get direction name from enum string representation
                    # Format: <E_Direction.FWD: 1> -> extract "FWD" -> map to "Fwd"
                    dir_str = str(direction)
                    dir_name = dir_str
                    if "." in dir_str and ":" in dir_str:
                        # Extract "FWD" from "<E_Direction.FWD: 1>"
                        dir_name = dir_str.split(".")[-1].split(":")[0].strip()
                    # Map to consistent naming (Fwd, Bwd, Left, Right)
                    DIRECTION_MAP = {
                        "IDLE": "Idle", "FWD": "Fwd", "FWD_LEFT": "FwdLeft", "FWD_RIGHT": "FwdRight",
                        "LEFT": "Left", "RIGHT": "Right", "BWD": "Bwd", "BWD_LEFT": "BwdLeft", "BWD_RIGHT": "BwdRight"
                    }
                    dir_name = DIRECTION_MAP.get(dir_name.upper(), dir_name)

                    # Get path from soft object reference
                    montage_path = ""
                    if montage_ref:
                        # TSoftObjectPtr - get the path
                        montage_path = str(montage_ref.get_asset_name()) if hasattr(montage_ref, 'get_asset_name') else str(montage_ref)
                        # Try to get full path
                        if hasattr(montage_ref, 'get_path_name'):
                            montage_path = montage_ref.get_path_name()
                        elif hasattr(montage_ref, 'to_soft_object_path'):
                            montage_path = str(montage_ref.to_soft_object_path())
                        else:
                            # Convert to string and extract path
                            montage_str = str(montage_ref)
                            if "'" in montage_str:
                                # Format: SoftObjectPath'/Game/...'
                                montage_path = montage_str.split("'")[1] if len(montage_str.split("'")) > 1 else montage_str

                    if montage_path and montage_path != "None":
                        asset_data["death_montages"][dir_name] = montage_path
                        unreal.log_warning(f"    {dir_name} -> {montage_path}")
            else:
                unreal.log_warning(f"  Death TMap is empty or None")
        except Exception as e:
            unreal.log_warning(f"  [ERROR] Could not get Death property: {e}")

        # Also try to get HitReactions property for reference
        try:
            hit_map = asset.get_editor_property("HitReactions")
            if hit_map:
                unreal.log_warning(f"  HitReactions TMap found with {len(hit_map)} entries")
                for direction, montage_ref in hit_map.items():
                    # Get direction name from enum string representation
                    # Format: <E_Direction.FWD: 1> -> extract "FWD" -> map to "Fwd"
                    dir_str = str(direction)
                    dir_name = dir_str
                    if "." in dir_str and ":" in dir_str:
                        dir_name = dir_str.split(".")[-1].split(":")[0].strip()
                    DIRECTION_MAP = {
                        "IDLE": "Idle", "FWD": "Fwd", "FWD_LEFT": "FwdLeft", "FWD_RIGHT": "FwdRight",
                        "LEFT": "Left", "RIGHT": "Right", "BWD": "Bwd", "BWD_LEFT": "BwdLeft", "BWD_RIGHT": "BwdRight"
                    }
                    dir_name = DIRECTION_MAP.get(dir_name.upper(), dir_name)

                    montage_path = ""
                    if montage_ref:
                        montage_str = str(montage_ref)
                        if "'" in montage_str:
                            montage_path = montage_str.split("'")[1] if len(montage_str.split("'")) > 1 else montage_str

                    if montage_path and montage_path != "None":
                        asset_data["hit_reactions"][dir_name] = montage_path
        except Exception as e:
            unreal.log_warning(f"  [NOTE] Could not get HitReactions: {e}")

        if asset_data["death_montages"] or asset_data["hit_reactions"]:
            result[asset_name] = asset_data

    # Save to cache
    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)
    with open(OUTPUT_FILE, 'w') as f:
        json.dump(result, f, indent=2)

    unreal.log_warning(f"\n[SAVED] {OUTPUT_FILE}")
    unreal.log_warning(f"  {len(result)} animsets extracted")

    return result


if __name__ == "__main__":
    extract_death_montages()
