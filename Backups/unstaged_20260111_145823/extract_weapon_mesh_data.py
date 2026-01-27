# extract_weapon_mesh_data.py
# Extracts StaticMesh component data from weapon Blueprints
# Run on SLFConversion project AFTER restoring backup but BEFORE migration
#
# Run with:
# UnrealEditor-Cmd.exe "C:/scripts/SLFConversion/SLFConversion.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/extract_weapon_mesh_data.py" -stdout -unattended

import unreal
import json

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/weapon_mesh_data.json"
DEBUG_FILE = "C:/scripts/SLFConversion/weapon_extract_debug.txt"

def debug_log(msg):
    """Write debug message to file"""
    with open(DEBUG_FILE, 'a') as f:
        f.write(msg + "\n")
    print(msg)

# Weapon Blueprint paths to extract
WEAPON_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample02",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Katana",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_BossMace",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Greatsword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_AI_Weapon",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon",
]

def extract_weapon_mesh_data():
    """
    Extract StaticMesh paths from weapon Blueprint SCS components.
    Uses direct Blueprint.simple_construction_script access (requires SLFConversion project).
    """
    # Clear debug file
    with open(DEBUG_FILE, 'w') as f:
        f.write("")

    debug_log("")
    debug_log("=" * 70)
    debug_log("EXTRACTING WEAPON MESH DATA (SCS direct access)")
    debug_log("=" * 70)

    weapon_data = {}

    for bp_path in WEAPON_BLUEPRINTS:
        bp_name = bp_path.split("/")[-1]
        debug_log(f"\n[{bp_name}]")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            debug_log(f"  [SKIP] Could not load")
            continue

        weapon_info = {
            "path": bp_path,
            "components": []
        }

        # Direct SCS access
        try:
            scs = bp.simple_construction_script
            if not scs:
                debug_log(f"  [SKIP] No SCS")
                continue

            nodes = scs.get_all_nodes()
            debug_log(f"  SCS has {len(nodes)} nodes")

            for node in nodes:
                comp = node.component_template
                if not comp:
                    continue

                comp_name = comp.get_name()
                comp_class = comp.get_class().get_name()
                debug_log(f"    - {comp_name}: {comp_class}")

                comp_info = {
                    "name": comp_name,
                    "class": comp_class,
                }

                # For StaticMeshComponent, get the mesh path
                if comp_class == "StaticMeshComponent":
                    try:
                        mesh = comp.get_editor_property('static_mesh')
                        if mesh:
                            mesh_path = mesh.get_path_name()
                            comp_info["static_mesh"] = mesh_path
                            debug_log(f"        Mesh: {mesh_path}")
                        else:
                            debug_log(f"        Mesh: None")
                    except Exception as e:
                        debug_log(f"        [ERROR] {e}")

                # For NiagaraComponent, get the asset
                elif comp_class == "NiagaraComponent":
                    try:
                        niagara_asset = comp.get_editor_property('asset')
                        if niagara_asset:
                            asset_path = niagara_asset.get_path_name()
                            comp_info["niagara_asset"] = asset_path
                            debug_log(f"        Asset: {asset_path}")
                    except Exception as e:
                        debug_log(f"        [ERROR] {e}")

                weapon_info["components"].append(comp_info)

            if weapon_info["components"]:
                weapon_data[bp_name] = weapon_info
            else:
                debug_log(f"  [NO COMPONENTS]")

        except Exception as e:
            debug_log(f"  [ERROR] SCS access failed: {e}")

    # Save to JSON
    debug_log("")
    debug_log("=" * 70)
    debug_log(f"SAVING TO {OUTPUT_FILE}")
    debug_log("=" * 70)

    with open(OUTPUT_FILE, 'w') as f:
        json.dump(weapon_data, f, indent=2)

    debug_log(f"\nExtracted data for {len(weapon_data)} weapons")
    for name in weapon_data:
        debug_log(f"  - {name}")

    return weapon_data


if __name__ == "__main__":
    extract_weapon_mesh_data()
