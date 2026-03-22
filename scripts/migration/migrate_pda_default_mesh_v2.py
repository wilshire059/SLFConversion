"""
Option B Migration v2: PDA_DefaultMeshData
Load actual SkeletalMesh assets and assign to TSoftObjectPtr properties.
"""
import unreal

results = []
def log(msg):
    results.append(msg)
    unreal.log(msg)

log("=" * 70)
log("OPTION B: PDA_DefaultMeshData Migration (v2 - Load Actual Assets)")
log("=" * 70)

eal = unreal.EditorAssetLibrary
bel = unreal.BlueprintEditorLibrary

# Asset paths
ASSET_PATH = "/Game/SoulslikeFramework/Data/PDA_DefaultMeshData"
CPP_CLASS_PATH = "/Script/SLFConversion.PDA_DefaultMeshData"

# Mesh paths extracted from backup (DA_QuinnMeshDefault data)
MESH_DATA = {
    'head_mesh': '/Game/SoulslikeFramework/Meshes/SKM/SKM_QuinnCape_Head',
    'upper_body_mesh': '/Game/SoulslikeFramework/Meshes/SKM/SKM_Quinn_UpperBody',
    'arms_mesh': '/Game/SoulslikeFramework/Meshes/SKM/SKM_Quinn_Arms',
    'lower_body_mesh': '/Game/SoulslikeFramework/Meshes/SKM/SKM_QuinnCape_LowerBody',
}

# Step 1: Load and verify all skeletal meshes exist
log("\n[Step 1] Loading skeletal mesh assets...")
loaded_meshes = {}
all_found = True
for prop_name, mesh_path in MESH_DATA.items():
    if eal.does_asset_exist(mesh_path):
        mesh = eal.load_asset(mesh_path)
        if mesh:
            log(f"  FOUND: {mesh_path} -> {mesh.get_class().get_name()}")
            loaded_meshes[prop_name] = mesh
        else:
            log(f"  ERROR: Could not load {mesh_path}")
            all_found = False
    else:
        log(f"  MISSING: {mesh_path}")
        all_found = False

if not all_found:
    log("\nERROR: Not all meshes found. Aborting migration.")
else:
    # Step 2: Load Blueprint asset
    log("\n[Step 2] Loading Blueprint asset...")
    if not eal.does_asset_exist(ASSET_PATH):
        log(f"ERROR: Asset not found: {ASSET_PATH}")
    else:
        bp = eal.load_asset(ASSET_PATH)
        if not bp:
            log("ERROR: Failed to load Blueprint asset")
        else:
            log(f"Loaded Blueprint: {bp.get_name()}")
            log(f"Current class: {bp.get_class().get_name()}")

            # Step 3: Load C++ parent class
            log("\n[Step 3] Loading C++ parent class...")
            cpp_class = unreal.load_class(None, CPP_CLASS_PATH)
            if not cpp_class:
                log(f"ERROR: Could not load C++ class: {CPP_CLASS_PATH}")
            else:
                log(f"C++ class loaded: {cpp_class.get_name()}")

                # Step 4: Reparent Blueprint to C++ class
                log("\n[Step 4] Reparenting Blueprint...")
                try:
                    success = bel.reparent_blueprint(bp, cpp_class)
                    if success:
                        log("SUCCESS: Blueprint reparented to C++ class")
                    else:
                        log("WARNING: Reparent returned false (may already be correct parent)")
                except Exception as e:
                    log(f"Reparent error: {e}")

                # Step 5: Apply mesh data to C++ properties
                log("\n[Step 5] Applying mesh data to C++ properties...")

                # Get the generated class and CDO
                gen_class = bp.generated_class()
                if gen_class:
                    cdo = unreal.get_default_object(gen_class)
                    if cdo:
                        log(f"Got CDO: {cdo}")

                        for prop_name, mesh in loaded_meshes.items():
                            try:
                                log(f"  Setting {prop_name} = {mesh.get_path_name()}")
                                # Set property with actual loaded mesh - UE handles soft ref conversion
                                cdo.set_editor_property(prop_name, mesh)
                                log(f"    SUCCESS: {prop_name} set")
                            except Exception as e:
                                log(f"    ERROR setting {prop_name}: {e}")
                    else:
                        log("ERROR: Could not get CDO")
                else:
                    log("ERROR: Could not get generated class")

                # Step 6: Mark dirty and save
                log("\n[Step 6] Saving asset...")
                try:
                    eal.save_asset(ASSET_PATH, only_if_is_dirty=False)
                    log("SUCCESS: Asset saved")
                except Exception as e:
                    log(f"Save error: {e}")

# Step 7: Verify
log("\n[Step 7] Verification...")
bp_check = eal.load_asset(ASSET_PATH)
if bp_check:
    gen = bp_check.generated_class()
    if gen:
        cdo = unreal.get_default_object(gen)
        if cdo:
            for prop_name in ['head_mesh', 'upper_body_mesh', 'arms_mesh', 'lower_body_mesh']:
                try:
                    val = cdo.get_editor_property(prop_name)
                    if val:
                        # For soft object ptr, try to get the path
                        try:
                            path = val.get_path_name() if hasattr(val, 'get_path_name') else str(val)
                            log(f"  {prop_name}: {path}")
                        except:
                            log(f"  {prop_name}: {val}")
                    else:
                        log(f"  {prop_name}: None")
                except Exception as e:
                    log(f"  {prop_name}: ERROR - {e}")

log("\n" + "=" * 70)
log("MIGRATION COMPLETE")
log("=" * 70)

# Write results
with open("C:/scripts/slfconversion/migration_results_v2.txt", "w") as f:
    f.write("\n".join(results))
