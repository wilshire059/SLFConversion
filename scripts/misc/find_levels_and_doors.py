"""
Find all level files and door-related assets in the project.
"""
import unreal
import json

results = {
    "levels": [],
    "door_assets": [],
    "b_door_status": {},
    "demo_contents": [],
    "level_folder_contents": []
}

unreal.log("=" * 70)
unreal.log("FINDING LEVELS AND DOORS")
unreal.log("=" * 70)

# Find all level/map assets
unreal.log("\n[1] SEARCHING FOR LEVEL ASSETS...")
all_assets = unreal.EditorAssetLibrary.list_assets("/Game/", recursive=True)

for asset in all_assets:
    asset_str = str(asset)
    # Look for World assets (levels)
    if asset_str.endswith("_C") == False:
        # Try to identify levels by name patterns
        if "L_" in asset_str or "Level" in asset_str or "_Map" in asset_str:
            # Verify it's actually a world/map
            asset_data = unreal.EditorAssetLibrary.find_asset_data(asset)
            if asset_data:
                class_name = str(asset_data.asset_class_path)
                if "World" in class_name:
                    results["levels"].append(asset_str)
                    unreal.log(f"  Level: {asset_str}")

# Find all door Blueprints
unreal.log("\n[2] SEARCHING FOR DOOR BLUEPRINTS...")
for asset in all_assets:
    asset_str = str(asset)
    if "Door" in asset_str and "_C" not in asset_str:
        results["door_assets"].append(asset_str)
        unreal.log(f"  Door: {asset_str}")

# Check B_Door specifically
unreal.log("\n[3] CHECKING B_Door CLASS...")
door_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"
door_bp = unreal.EditorAssetLibrary.load_asset(door_path)

if door_bp:
    unreal.log(f"B_Door loaded successfully")
    gen_class = door_bp.generated_class()
    if gen_class:
        results["b_door_status"]["class_name"] = gen_class.get_name()

        # Get CDO
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            results["b_door_status"]["cdo_components"] = []
            mesh_comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
            for comp in mesh_comps:
                mesh = comp.static_mesh
                visible = comp.is_visible()
                comp_info = {
                    "name": comp.get_name(),
                    "mesh": mesh.get_name() if mesh else "NONE",
                    "visible": visible
                }
                results["b_door_status"]["cdo_components"].append(comp_info)
                unreal.log(f"  [{comp.get_name()}] Mesh: {comp_info['mesh']}, Visible: {visible}")

            # Check TSoftObjectPtr properties
            try:
                default_door = cdo.get_editor_property('default_door_mesh')
                default_frame = cdo.get_editor_property('default_door_frame_mesh')
                results["b_door_status"]["default_door_mesh"] = str(default_door) if default_door else "NOT_SET"
                results["b_door_status"]["default_door_frame_mesh"] = str(default_frame) if default_frame else "NOT_SET"
                unreal.log(f"  DefaultDoorMesh: {default_door}")
                unreal.log(f"  DefaultDoorFrameMesh: {default_frame}")
            except Exception as e:
                results["b_door_status"]["softptr_error"] = str(e)
                unreal.log(f"  Could not get TSoftObjectPtr: {e}")

# List Demo folder
unreal.log("\n[4] DEMO FOLDER...")
demo_assets = unreal.EditorAssetLibrary.list_assets("/Game/SoulslikeFramework/Demo/", recursive=True)
for asset in demo_assets:
    asset_str = str(asset)
    if "_Level" in asset_str or "L_" in asset_str:
        results["demo_contents"].append(asset_str)
        unreal.log(f"  {asset_str}")

# Check _Levels folder specifically
unreal.log("\n[5] CHECKING _LEVELS FOLDER...")
if unreal.EditorAssetLibrary.does_directory_exist("/Game/SoulslikeFramework/Demo/_Levels"):
    results["level_folder_exists"] = True
    level_contents = unreal.EditorAssetLibrary.list_assets("/Game/SoulslikeFramework/Demo/_Levels/", recursive=True)
    for lc in level_contents:
        results["level_folder_contents"].append(str(lc))
        unreal.log(f"  {lc}")
else:
    results["level_folder_exists"] = False
    unreal.log("  _Levels folder does NOT exist")

# Save results
output_path = "C:/scripts/SLFConversion/migration_cache/levels_and_doors.json"
with open(output_path, "w") as f:
    json.dump(results, f, indent=2)
unreal.log(f"\nSaved to {output_path}")
