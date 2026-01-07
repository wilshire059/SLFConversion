"""
Option B Migration: PDA_DefaultMeshData
Reparent to C++ class and apply mesh data directly to C++ properties.
"""
import unreal

results = []
def log(msg):
    results.append(msg)
    unreal.log(msg)

log("=" * 70)
log("OPTION B: PDA_DefaultMeshData Migration")
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

# Step 1: Load assets
log("\n[Step 1] Loading assets...")
if not eal.does_asset_exist(ASSET_PATH):
    log(f"ERROR: Asset not found: {ASSET_PATH}")
else:
    bp = eal.load_asset(ASSET_PATH)
    if not bp:
        log("ERROR: Failed to load Blueprint asset")
    else:
        log(f"Loaded Blueprint: {bp.get_name()}")
        log(f"Current class: {bp.get_class().get_name()}")

        # Step 2: Load C++ parent class
        log("\n[Step 2] Loading C++ parent class...")
        cpp_class = unreal.load_class(None, CPP_CLASS_PATH)
        if not cpp_class:
            log(f"ERROR: Could not load C++ class: {CPP_CLASS_PATH}")
        else:
            log(f"C++ class loaded: {cpp_class.get_name()}")

            # Step 3: Reparent Blueprint to C++ class
            log("\n[Step 3] Reparenting Blueprint...")
            try:
                success = bel.reparent_blueprint(bp, cpp_class)
                if success:
                    log("SUCCESS: Blueprint reparented to C++ class")
                else:
                    log("WARNING: Reparent returned false (may already be correct parent)")
            except Exception as e:
                log(f"Reparent error: {e}")

            # Step 4: Apply mesh data to direct C++ properties
            log("\n[Step 4] Applying mesh data to C++ properties...")

            # Get the generated class and CDO
            gen_class = bp.generated_class()
            if gen_class:
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    log(f"Got CDO: {cdo}")

                    for prop_name, mesh_path in MESH_DATA.items():
                        try:
                            # Load the skeletal mesh
                            full_mesh_path = mesh_path + ".SkeletalMesh'" + mesh_path.split('/')[-1] + "'"
                            # Try loading as soft reference
                            log(f"  Setting {prop_name} = {mesh_path}")

                            # For TSoftObjectPtr, we set with a string path
                            # The property expects the full path
                            mesh_ref = unreal.SoftObjectPath(mesh_path)
                            cdo.set_editor_property(prop_name, mesh_ref)

                            log(f"    SUCCESS: {prop_name} set")
                        except Exception as e:
                            log(f"    ERROR setting {prop_name}: {e}")
                else:
                    log("ERROR: Could not get CDO")
            else:
                log("ERROR: Could not get generated class")

            # Step 5: Mark dirty and save
            log("\n[Step 5] Saving asset...")
            try:
                eal.save_asset(ASSET_PATH, only_if_is_dirty=False)
                log("SUCCESS: Asset saved")
            except Exception as e:
                log(f"Save error: {e}")

# Step 6: Verify
log("\n[Step 6] Verification...")
bp_check = eal.load_asset(ASSET_PATH)
if bp_check:
    gen = bp_check.generated_class()
    if gen:
        cdo = unreal.get_default_object(gen)
        if cdo:
            for prop_name in ['head_mesh', 'upper_body_mesh', 'arms_mesh', 'lower_body_mesh']:
                try:
                    val = cdo.get_editor_property(prop_name)
                    log(f"  {prop_name}: {val}")
                except Exception as e:
                    log(f"  {prop_name}: ERROR - {e}")

log("\n" + "=" * 70)
log("MIGRATION COMPLETE")
log("=" * 70)

# Write results
with open("C:/scripts/slfconversion/migration_results.txt", "w") as f:
    f.write("\n".join(results))
