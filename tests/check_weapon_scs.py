# check_weapon_scs.py
# Check if weapon Blueprints have their SCS (SimpleConstructionScript) components

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/weapon_scs_output.txt"

def run():
    results = []

    def log(msg):
        results.append(msg)
        unreal.log_warning(msg)

    def section(title):
        log("")
        log("=" * 70)
        log(title)
        log("=" * 70)

    section("WEAPON BLUEPRINT SCS DIAGNOSTIC")

    weapon_bps = [
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/B_Item",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_AI_Weapon",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword",
    ]

    for bp_path in weapon_bps:
        bp_name = bp_path.split("/")[-1]
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)

        if not bp:
            log(f"\n[{bp_name}]: NOT FOUND")
            continue

        log(f"\n[{bp_name}]")

        # Get parent class
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"  Parent: {parent}")

        # Check SCS (SimpleConstructionScript)
        try:
            scs = bp.get_editor_property('simple_construction_script')
            if scs:
                all_nodes = scs.get_all_nodes()
                log(f"  SCS Nodes: {len(all_nodes)}")

                for node in all_nodes:
                    comp_template = node.component_template
                    if comp_template:
                        comp_name = comp_template.get_name()
                        comp_class = comp_template.get_class().get_name()
                        log(f"    - {comp_name} ({comp_class})")

                        # For StaticMeshComponent, check mesh assignment
                        if "StaticMesh" in comp_class:
                            try:
                                static_mesh = comp_template.get_editor_property('static_mesh')
                                if static_mesh:
                                    log(f"        Mesh: {static_mesh.get_name()}")
                                else:
                                    log(f"        Mesh: None (NOT SET!)")
                            except Exception as e:
                                log(f"        Mesh error: {e}")
            else:
                log(f"  SCS: None (NO BLUEPRINT COMPONENTS!)")
        except Exception as e:
            log(f"  SCS Error: {e}")

    # =========================================================================
    # CHECK INHERITED COMPONENTS
    # =========================================================================
    section("INHERITED COMPONENTS (from C++ parent)")

    for bp_path in weapon_bps[:2]:  # Just check B_Item and B_Item_Weapon
        bp_name = bp_path.split("/")[-1]
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)

        if not bp:
            continue

        log(f"\n[{bp_name}]")

        try:
            bp_class = bp.generated_class()
            if bp_class:
                cdo = unreal.get_default_object(bp_class)
                if cdo:
                    # Get all components
                    all_comps = cdo.get_components_by_class(unreal.ActorComponent)
                    log(f"  Total Components on CDO: {len(all_comps)}")
                    for comp in all_comps:
                        log(f"    - {comp.get_name()} ({comp.get_class().get_name()})")
        except Exception as e:
            log(f"  Error: {e}")

    # =========================================================================
    # DIAGNOSIS
    # =========================================================================
    section("DIAGNOSIS")

    log("")
    log("If SCS shows no components or 'StaticMesh' is missing:")
    log("  -> The migration cleared the Blueprint's component hierarchy")
    log("")
    log("SOLUTIONS:")
    log("  1. Restore weapon Blueprints from backup:")
    log("     C:\\scripts\\SLFConversion_Migration\\Backups\\blueprint_only\\")
    log("")
    log("  2. Add weapons to KEEP_VARS_MAP in run_migration.py")
    log("     This preserves Blueprint components during migration")
    log("")
    log("  3. Add StaticMeshComponent to C++ SLFItemBase parent class")

    # Write output
    output = "\n".join(results)
    with open(OUTPUT_FILE, 'w') as f:
        f.write(output)

run()
