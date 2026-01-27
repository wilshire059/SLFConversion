# apply_weapon_mesh_data.py
# Restores StaticMesh components to weapon Blueprints after reparenting
# Run AFTER migration has reparented weapons to C++ classes
#
# Run with:
# UnrealEditor-Cmd.exe "C:/scripts/SLFConversion/SLFConversion.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/apply_weapon_mesh_data.py" -stdout -unattended

import unreal
import json

CACHE_FILE = "C:/scripts/SLFConversion/migration_cache/weapon_mesh_data.json"

def apply_weapon_mesh_data():
    """
    Restore StaticMesh components to weapon Blueprints from cached data.
    Creates new SCS StaticMeshComponent nodes and sets their mesh.
    """
    print("")
    print("=" * 70)
    print("APPLYING WEAPON MESH DATA FROM CACHE")
    print("=" * 70)

    # Load cached data
    try:
        with open(CACHE_FILE, 'r') as f:
            weapon_data = json.load(f)
    except FileNotFoundError:
        print(f"ERROR: Cache file not found: {CACHE_FILE}")
        print("Run extract_weapon_mesh_data.py on backup first!")
        return
    except json.JSONDecodeError as e:
        print(f"ERROR: Invalid JSON in cache: {e}")
        return

    if not weapon_data:
        print("ERROR: Cache is empty")
        return

    print(f"\nLoaded data for {len(weapon_data)} weapons")

    success_count = 0
    fail_count = 0

    for bp_name, data in weapon_data.items():
        bp_path = data.get("path", "")
        components = data.get("components", [])

        print(f"\n[{bp_name}]")
        print(f"  Path: {bp_path}")

        # Load the Blueprint
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            print(f"  [SKIP] Could not load Blueprint")
            fail_count += 1
            continue

        # Get or create SCS
        scs = bp.simple_construction_script
        if not scs:
            print(f"  [SKIP] No SimpleConstructionScript")
            fail_count += 1
            continue

        modified = False

        for comp_data in components:
            comp_name = comp_data.get("name", "")
            comp_class = comp_data.get("class", "")
            static_mesh_path = comp_data.get("static_mesh", "")
            niagara_path = comp_data.get("niagara_asset", "")

            print(f"  Processing: {comp_name} ({comp_class})")

            # Check if component already exists
            existing_nodes = scs.get_all_nodes()
            exists = False
            for node in existing_nodes:
                if str(node.get_variable_name()) == comp_name:
                    exists = True
                    comp_template = node.component_template

                    # Update existing component
                    if comp_class == "StaticMeshComponent" and static_mesh_path:
                        mesh = unreal.EditorAssetLibrary.load_asset(static_mesh_path)
                        if mesh:
                            comp_template.set_editor_property('static_mesh', mesh)
                            print(f"    [OK] Updated existing mesh: {static_mesh_path}")
                            modified = True
                        else:
                            print(f"    [WARN] Could not load mesh: {static_mesh_path}")

                    elif comp_class == "NiagaraComponent" and niagara_path:
                        asset = unreal.EditorAssetLibrary.load_asset(niagara_path)
                        if asset:
                            comp_template.set_editor_property('asset', asset)
                            print(f"    [OK] Updated existing niagara: {niagara_path}")
                            modified = True
                    break

            if not exists:
                # Need to create new component
                print(f"    Component doesn't exist, creating new...")

                if comp_class == "StaticMeshComponent" and static_mesh_path:
                    # Create StaticMeshComponent via SLFAutomationLibrary
                    smc_class = unreal.StaticMeshComponent.static_class()
                    result = unreal.SLFAutomationLibrary.add_scs_component(bp, smc_class, comp_name)

                    if result:
                        print(f"    [OK] Created StaticMeshComponent: {comp_name}")

                        # Now find and set the mesh
                        updated_nodes = scs.get_all_nodes()
                        for node in updated_nodes:
                            if str(node.get_variable_name()) == comp_name:
                                mesh = unreal.EditorAssetLibrary.load_asset(static_mesh_path)
                                if mesh and node.component_template:
                                    node.component_template.set_editor_property('static_mesh', mesh)
                                    print(f"    [OK] Set mesh: {static_mesh_path}")
                                    modified = True
                                break
                    else:
                        print(f"    [FAIL] Could not create component")

                elif comp_class == "NiagaraComponent" and niagara_path:
                    nc_class = unreal.NiagaraComponent.static_class()
                    result = unreal.SLFAutomationLibrary.add_scs_component(bp, nc_class, comp_name)

                    if result:
                        print(f"    [OK] Created NiagaraComponent: {comp_name}")

                        updated_nodes = scs.get_all_nodes()
                        for node in updated_nodes:
                            if str(node.get_variable_name()) == comp_name:
                                asset = unreal.EditorAssetLibrary.load_asset(niagara_path)
                                if asset and node.component_template:
                                    node.component_template.set_editor_property('asset', asset)
                                    print(f"    [OK] Set asset: {niagara_path}")
                                    modified = True
                                break

        # Save if modified
        if modified:
            # Compile Blueprint
            unreal.KismetEditorUtilities.compile_blueprint(bp)
            # Save
            unreal.EditorAssetLibrary.save_asset(bp_path)
            print(f"  [SAVED] {bp_name}")
            success_count += 1
        else:
            print(f"  [NO CHANGES] {bp_name}")

    print("")
    print("=" * 70)
    print(f"COMPLETE: {success_count} succeeded, {fail_count} failed")
    print("=" * 70)


if __name__ == "__main__":
    apply_weapon_mesh_data()
