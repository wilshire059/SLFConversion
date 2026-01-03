# check_weapon_spawn.py
# Check if weapons are actually spawning as child actors

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/weapon_spawn_output.txt"

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

    section("WEAPON SPAWN DIAGNOSTIC")

    # Load level
    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    try:
        unreal.EditorLevelLibrary.load_level(level_path)
        subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
        world = subsystem.get_editor_world()
    except Exception as e:
        log(f"Level error: {e}")
        return

    if not world:
        log("Could not get world")
        return

    log(f"Level: {world.get_name()}")

    # Get all characters
    characters = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Character)

    for char in characters:
        char_name = char.get_name()
        if "Enemy" not in char_name and "Boss" not in char_name:
            continue

        log("")
        log(f"[{char_name}]")
        log(f"  Class: {char.get_class().get_name()}")

        # Get ChildActorComponents
        child_actor_comps = char.get_components_by_class(unreal.ChildActorComponent)
        log(f"  ChildActorComponents: {len(child_actor_comps)}")

        for cac in child_actor_comps:
            comp_name = cac.get_name()
            log(f"")
            log(f"    [{comp_name}]")

            # ChildActorClass
            try:
                child_class = cac.get_editor_property("ChildActorClass")
                if child_class:
                    log(f"      ChildActorClass: {child_class.get_name()}")
                else:
                    log(f"      ChildActorClass: None (NOT SET!)")
            except Exception as e:
                log(f"      ChildActorClass error: {e}")

            # ChildActorTemplate
            try:
                template = cac.get_editor_property("ChildActorTemplate")
                if template:
                    log(f"      ChildActorTemplate: {template.get_name()}")
                else:
                    log(f"      ChildActorTemplate: None")
            except:
                pass

            # ChildActor (the spawned instance)
            try:
                child = cac.get_child_actor()
                if child:
                    log(f"      ChildActor: {child.get_name()} (WEAPON EXISTS!)")
                    log(f"        Class: {child.get_class().get_name()}")
                    loc = child.get_actor_location()
                    log(f"        Location: ({loc.x:.0f}, {loc.y:.0f}, {loc.z:.0f})")

                    # Check visibility
                    try:
                        hidden = child.get_editor_property("bHidden")
                        log(f"        Hidden: {hidden}")
                    except:
                        pass

                    # Check mesh component
                    try:
                        mesh_comps = child.get_components_by_class(unreal.StaticMeshComponent)
                        if mesh_comps:
                            log(f"        StaticMeshComponents: {len(mesh_comps)}")
                            for mc in mesh_comps:
                                mesh = mc.get_editor_property("StaticMesh")
                                visible = mc.is_visible()
                                log(f"          - {mc.get_name()}: Mesh={mesh.get_name() if mesh else 'None'}, Visible={visible}")
                    except:
                        pass

                else:
                    log(f"      ChildActor: None (WEAPON NOT SPAWNED!)")
            except Exception as e:
                log(f"      ChildActor error: {e}")

            # Check attachment
            try:
                attach_parent = cac.get_attach_parent()
                if attach_parent:
                    log(f"      AttachParent: {attach_parent.get_name()}")
                socket = cac.get_attach_socket_name()
                if socket and str(socket) != "None":
                    log(f"      AttachSocket: {socket}")
            except:
                pass

    # =========================================================================
    # CHECK WEAPON BLUEPRINT COMPILATION
    # =========================================================================
    section("WEAPON BLUEPRINT STATUS")

    weapon_bps = [
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_AI_Weapon",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Greatsword",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_BossMace",
    ]

    for bp_path in weapon_bps:
        bp_name = bp_path.split("/")[-1]
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)

        if not bp:
            log(f"  {bp_name}: NOT FOUND")
            continue

        log(f"")
        log(f"[{bp_name}]")

        # Parent class
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"  Parent: {parent}")

        # Blueprint status
        try:
            status = bp.get_editor_property("Status")
            log(f"  Status: {status}")
        except:
            pass

        # Try to compile
        try:
            # Get generated class
            gen_class = bp.generated_class()
            if gen_class:
                log(f"  GeneratedClass: {gen_class.get_name()}")

                # Get CDO
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    log(f"  CDO: OK")

                    # Check mesh
                    try:
                        mesh_comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
                        log(f"  StaticMeshComponents: {len(mesh_comps)}")
                        for mc in mesh_comps:
                            mesh = mc.get_editor_property("StaticMesh")
                            log(f"    - {mc.get_name()}: {mesh.get_name() if mesh else 'None'}")
                    except:
                        pass
                else:
                    log(f"  CDO: FAILED")
            else:
                log(f"  GeneratedClass: FAILED")
        except Exception as e:
            log(f"  Error: {e}")

    # =========================================================================
    # DIAGNOSIS
    # =========================================================================
    section("ANALYSIS")

    log("")
    log("ChildActorComponents in editor do not spawn their child actors")
    log("until runtime (PIE). In the editor, only the template exists.")
    log("")
    log("SOLUTION: Test in PIE (Play In Editor) to see if weapons spawn.")
    log("")
    log("If weapons still don't appear in PIE, possible issues:")
    log("  1. Weapon Blueprint has compilation errors")
    log("  2. Weapon mesh is not assigned")
    log("  3. Weapon is hidden or has scale 0")
    log("  4. Attachment socket doesn't exist on skeleton")

    # Write output
    output = "\n".join(results)
    with open(OUTPUT_FILE, 'w') as f:
        f.write(output)

run()
