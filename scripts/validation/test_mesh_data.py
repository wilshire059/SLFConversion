"""
Test if mesh data is accessible after redirect fix.
"""
import unreal

def log(msg):
    unreal.log(msg)
    print(msg)

log("=" * 70)
log("MESH DATA LOAD TEST")
log("=" * 70)

eal = unreal.EditorAssetLibrary

# Test PDA_DefaultMeshData
ASSET_PATH = "/Game/SoulslikeFramework/Data/PDA_DefaultMeshData"

log(f"\nTesting: {ASSET_PATH}")

if eal.does_asset_exist(ASSET_PATH):
    asset = eal.load_asset(ASSET_PATH)
    if asset:
        log(f"Loaded: {asset.get_name()}")
        log(f"Class: {asset.get_class().get_name()}")

        # List all properties
        log("\n--- All Properties ---")
        for prop_name in dir(asset):
            if not prop_name.startswith('_'):
                try:
                    # Try snake_case version
                    val = asset.get_editor_property(prop_name)
                    if val is not None:
                        log(f"  {prop_name}: {type(val).__name__} = {val}")
                except:
                    pass

        # Try common mesh property names
        log("\n--- Mesh Property Search ---")
        possible_props = [
            'mesh_data', 'default_mesh_data', 'MeshData', 'DefaultMeshData',
            'quinn_mesh', 'manny_mesh', 'skeletal_mesh_data', 'character_mesh',
            'head_mesh', 'body_mesh', 'upper_body_mesh', 'lower_body_mesh'
        ]

        for prop in possible_props:
            try:
                val = asset.get_editor_property(prop)
                log(f"  Found property '{prop}': {val}")
                if val is not None:
                    # Try to get nested mesh references
                    for nested in ['head', 'body', 'upper_body', 'lower_body', 'arms']:
                        try:
                            nested_val = val.get_editor_property(nested)
                            log(f"    .{nested}: {nested_val}")
                        except:
                            pass
            except Exception as e:
                pass  # Property doesn't exist with this name
    else:
        log("Failed to load asset")
else:
    log(f"Asset does not exist: {ASSET_PATH}")

# Also check character for mesh
log("\n" + "=" * 70)
log("CHARACTER MESH CHECK")
log("=" * 70)

CHAR_PATH = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
if eal.does_asset_exist(CHAR_PATH):
    bp = eal.load_asset(CHAR_PATH)
    if bp:
        log(f"Loaded: {bp.get_name()}")
        gen_class = bp.generated_class()
        if gen_class:
            log(f"Generated class: {gen_class.get_name()}")
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                log(f"Got CDO")
                # Check mesh component
                try:
                    mesh = cdo.get_editor_property('mesh')
                    log(f"Mesh component: {mesh}")
                    if mesh:
                        sk_mesh = mesh.get_editor_property('skeletal_mesh_asset')
                        log(f"SkeletalMesh: {sk_mesh}")
                        if sk_mesh:
                            log(f"Mesh path: {sk_mesh.get_path_name()}")
                except Exception as e:
                    log(f"Mesh access error: {e}")

log("\n" + "=" * 70)
log("TEST COMPLETE")
log("=" * 70)
