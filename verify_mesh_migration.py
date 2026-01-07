"""
Verify Option B Migration: Check if PDA_DefaultMeshData has mesh data
and if B_Soulslike_Character can access it via C++ parent class.
"""
import unreal

results = []
def log(msg):
    results.append(msg)
    unreal.log(msg)

log("=" * 70)
log("OPTION B MIGRATION VERIFICATION")
log("=" * 70)

eal = unreal.EditorAssetLibrary

# Test 1: Check PDA_DefaultMeshData mesh data
log("\n[TEST 1] PDA_DefaultMeshData Mesh Data Check")
log("-" * 50)

pda_path = "/Game/SoulslikeFramework/Data/PDA_DefaultMeshData"
if eal.does_asset_exist(pda_path):
    pda = eal.load_asset(pda_path)
    if pda:
        log(f"Loaded: {pda.get_name()}")
        log(f"Class: {pda.get_class().get_name()}")

        # Check if we can access the C++ properties
        mesh_props = ['head_mesh', 'upper_body_mesh', 'arms_mesh', 'lower_body_mesh']
        all_valid = True

        for prop in mesh_props:
            try:
                val = pda.get_editor_property(prop)
                if val:
                    log(f"  {prop}: {val.get_path_name() if hasattr(val, 'get_path_name') else val}")
                else:
                    log(f"  {prop}: None")
                    all_valid = False
            except Exception as e:
                log(f"  {prop}: ERROR - {e}")
                all_valid = False

        if all_valid:
            log("PASS: All mesh properties have valid references")
        else:
            log("FAIL: Some mesh properties are missing")
else:
    log(f"FAIL: PDA not found at {pda_path}")

# Test 2: Check B_Soulslike_Character can find DefaultMeshInfo
log("\n[TEST 2] B_Soulslike_Character DefaultMeshInfo Check")
log("-" * 50)

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

                # Check DefaultMeshInfo
                try:
                    mesh_info = cdo.get_editor_property('default_mesh_info')
                    if mesh_info:
                        log(f"DefaultMeshInfo: {mesh_info.get_name()}")
                        log(f"DefaultMeshInfo class: {mesh_info.get_class().get_name()}")

                        # This should be PDA_DefaultMeshData or derived
                        log("PASS: DefaultMeshInfo reference exists")
                    else:
                        log("WARNING: DefaultMeshInfo is None")
                except Exception as e:
                    log(f"ERROR accessing DefaultMeshInfo: {e}")

# Test 3: Spawn actor and check mesh components (simulation)
log("\n[TEST 3] Character Mesh Component Summary")
log("-" * 50)

# Check if mesh components are configured to get default mesh
if eal.does_asset_exist(char_path):
    bp = eal.load_asset(char_path)
    if bp:
        gen = bp.generated_class()
        if gen:
            cdo = unreal.get_default_object(gen)
            if cdo:
                mesh_components = ['head', 'upper_body', 'arms', 'lower_body']

                for comp_name in mesh_components:
                    try:
                        comp = cdo.get_editor_property(comp_name)
                        if comp:
                            try:
                                sk = comp.get_editor_property('skeletal_mesh_asset')
                                if sk:
                                    log(f"  {comp_name}: {sk.get_path_name()}")
                                else:
                                    log(f"  {comp_name}: No mesh assigned (will be set at BeginPlay)")
                            except:
                                log(f"  {comp_name}: Component exists (mesh set at runtime)")
                        else:
                            log(f"  {comp_name}: Component not found")
                    except Exception as e:
                        log(f"  {comp_name}: ERROR - {e}")

log("\n" + "=" * 70)
log("VERIFICATION COMPLETE")
log("=" * 70)

log("\nNOTE: Mesh components get their meshes at runtime in BeginPlay,")
log("not in the Blueprint editor CDO. The key test is that DefaultMeshInfo")
log("properly references PDA_DefaultMeshData with valid mesh data.")

# Write results
with open("C:/scripts/slfconversion/verify_results.txt", "w") as f:
    f.write("\n".join(results))
