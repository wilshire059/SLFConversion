"""Test mesh data and write results to file"""
import unreal

results = []

def log(msg):
    results.append(msg)
    unreal.log(msg)

log("=" * 60)
log("MESH DATA VALIDATION TEST")
log("=" * 60)

eal = unreal.EditorAssetLibrary

# Test PDA_DefaultMeshData
path = "/Game/SoulslikeFramework/Data/PDA_DefaultMeshData"
log(f"Loading: {path}")

if eal.does_asset_exist(path):
    asset = eal.load_asset(path)
    if asset:
        log(f"SUCCESS: Loaded {asset.get_name()}")
        log(f"Class: {asset.get_class().get_name()}")

        # Try property access using Python naming conventions
        props = ['manny_mesh_data', 'quinn_mesh_data', 'mesh_data', 'default_mesh_data']
        found_any = False
        for p in props:
            try:
                val = asset.get_editor_property(p)
                if val is not None:
                    log(f"FOUND: {p} = {val}")
                    found_any = True
                    # Try nested access
                    for nested in ['head_mesh', 'upper_body_mesh', 'lower_body_mesh', 'arms_mesh']:
                        try:
                            n = val.get_editor_property(nested)
                            if n:
                                log(f"  {nested}: {n.get_path_name() if hasattr(n, 'get_path_name') else n}")
                        except:
                            pass
            except:
                pass

        if not found_any:
            log("NO mesh data properties found!")
    else:
        log("FAILED to load asset")
else:
    log(f"Asset does not exist: {path}")

# Check character mesh
log("\n" + "=" * 60)
log("CHARACTER MESH CHECK")
log("=" * 60)

char_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
if eal.does_asset_exist(char_path):
    bp = eal.load_asset(char_path)
    if bp:
        log(f"Loaded: {bp.get_name()}")
        gen = bp.generated_class()
        if gen:
            cdo = unreal.get_default_object(gen)
            if cdo:
                log("Got CDO successfully")
                # Try to get mesh component
                try:
                    mesh = cdo.get_editor_property('mesh')
                    log(f"Mesh component: {mesh}")
                    if mesh:
                        try:
                            sk = mesh.get_editor_property('skeletal_mesh_asset')
                            if sk:
                                log(f"HAS SKELETAL MESH: {sk.get_path_name()}")
                            else:
                                log("SKELETAL MESH IS NONE - This is the problem!")
                        except Exception as e:
                            log(f"SkeletalMesh property error: {e}")

                        # Also check if it's using SkeletalMesh (old property name)
                        try:
                            sk2 = mesh.get_editor_property('skeletal_mesh')
                            if sk2:
                                log(f"Using old skeletal_mesh property: {sk2}")
                        except:
                            pass
                except Exception as e:
                    log(f"Mesh component error: {e}")

log("\n" + "=" * 60)
log("TEST COMPLETE")
log("=" * 60)

# Write results to file
with open("C:/scripts/slfconversion/mesh_test_results.txt", "w") as f:
    f.write("\n".join(results))
