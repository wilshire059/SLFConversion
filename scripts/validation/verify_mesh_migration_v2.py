"""
Verify Option B Migration: Check if PDA_DefaultMeshData has mesh data
"""
import unreal

results = []
def log(msg):
    results.append(msg)
    unreal.log(msg)

log("=" * 70)
log("OPTION B MIGRATION VERIFICATION (v2)")
log("=" * 70)

eal = unreal.EditorAssetLibrary

# Test 1: Check PDA_DefaultMeshData mesh data via CDO
log("\n[TEST 1] PDA_DefaultMeshData Mesh Data Check")
log("-" * 50)

pda_path = "/Game/SoulslikeFramework/Data/PDA_DefaultMeshData"
if eal.does_asset_exist(pda_path):
    bp = eal.load_asset(pda_path)
    if bp:
        log(f"Loaded Blueprint: {bp.get_name()}")

        # Get the generated class to access C++ properties
        gen_class = bp.generated_class()
        if gen_class:
            log(f"Generated class: {gen_class.get_name()}")

            # Get CDO to access properties
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                log(f"CDO: {cdo}")

                # Check C++ properties via CDO
                mesh_props = ['head_mesh', 'upper_body_mesh', 'arms_mesh', 'lower_body_mesh']
                all_valid = True

                for prop in mesh_props:
                    try:
                        val = cdo.get_editor_property(prop)
                        if val:
                            path = val.get_path_name() if hasattr(val, 'get_path_name') else str(val)
                            log(f"  {prop}: {path}")
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
                log("ERROR: Could not get CDO")
        else:
            log("ERROR: Could not get generated class")
else:
    log(f"FAIL: PDA not found at {pda_path}")

# Test 2: Verify C++ class hierarchy
log("\n[TEST 2] Class Hierarchy Check")
log("-" * 50)

# Load the C++ parent class directly
cpp_class_path = "/Script/SLFConversion.PDA_DefaultMeshData"
cpp_class = unreal.load_class(None, cpp_class_path)
if cpp_class:
    log(f"C++ class: {cpp_class.get_name()}")

    # Check if the Blueprint's parent is correctly set
    if eal.does_asset_exist(pda_path):
        bp = eal.load_asset(pda_path)
        if bp:
            gen_class = bp.generated_class()
            if gen_class:
                # Check parent class
                parent = gen_class.get_super_class()
                if parent:
                    log(f"Blueprint parent: {parent.get_name()}")
                    if parent.get_name() == "PDA_DefaultMeshData":
                        log("PASS: Blueprint correctly inherits from C++ class")
                    else:
                        log(f"WARNING: Parent is {parent.get_name()}, not PDA_DefaultMeshData")
else:
    log(f"ERROR: Could not load C++ class {cpp_class_path}")

# Test 3: Check what B_Soulslike_Character references
log("\n[TEST 3] B_Soulslike_Character Check")
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

                # Try to access DefaultMeshInfo
                # Note: The property name in Blueprint might be different from C++
                possible_names = ['default_mesh_info', 'DefaultMeshInfo', 'DefaultMeshData']
                found = False
                for name in possible_names:
                    try:
                        mesh_info = cdo.get_editor_property(name)
                        if mesh_info:
                            log(f"Found DefaultMeshInfo via '{name}': {mesh_info.get_name()}")
                            found = True
                            break
                    except:
                        pass

                if not found:
                    log("DefaultMeshInfo not directly accessible via CDO")
                    log("(This is expected - it may be set in the Blueprint editor)")

log("\n" + "=" * 70)
log("VERIFICATION COMPLETE")
log("=" * 70)

# Write results
with open("C:/scripts/slfconversion/verify_results_v2.txt", "w") as f:
    f.write("\n".join(results))
