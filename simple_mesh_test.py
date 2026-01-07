"""Simple test to check if mesh data loads"""
import unreal

unreal.log("=" * 60)
unreal.log("SIMPLE MESH DATA TEST")
unreal.log("=" * 60)

eal = unreal.EditorAssetLibrary

# Test PDA_DefaultMeshData
path = "/Game/SoulslikeFramework/Data/PDA_DefaultMeshData"
unreal.log(f"Loading: {path}")

if eal.does_asset_exist(path):
    asset = eal.load_asset(path)
    if asset:
        unreal.log(f"SUCCESS: Loaded {asset.get_name()}")
        unreal.log(f"Class: {asset.get_class().get_name()}")

        # Try all common properties
        props_to_try = [
            'manny_mesh_data', 'quinn_mesh_data', 'mesh_data',
            'default_mesh_data', 'manny_mesh', 'quinn_mesh'
        ]
        for p in props_to_try:
            try:
                val = asset.get_editor_property(p)
                unreal.log(f"FOUND PROPERTY '{p}': {val}")
            except:
                pass
    else:
        unreal.log("FAILED: Could not load asset")
else:
    unreal.log("FAILED: Asset does not exist")

# Check character
char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
unreal.log(f"\nLoading character: {char_path}")

if eal.does_asset_exist(char_path):
    bp = eal.load_asset(char_path)
    if bp:
        unreal.log(f"SUCCESS: Loaded {bp.get_name()}")
        gen = bp.generated_class()
        if gen:
            cdo = unreal.get_default_object(gen)
            if cdo:
                unreal.log("Got CDO, checking mesh...")
                try:
                    mesh = cdo.get_editor_property('mesh')
                    unreal.log(f"Mesh component: {mesh}")
                    if mesh:
                        sk = mesh.get_editor_property('skeletal_mesh_asset')
                        unreal.log(f"SkeletalMeshAsset: {sk}")
                except Exception as e:
                    unreal.log(f"Error: {e}")

unreal.log("=" * 60)
unreal.log("TEST COMPLETE")
unreal.log("=" * 60)
