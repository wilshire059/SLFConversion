# check_weapon_scs_detailed.py
# Check SCS components in weapon Blueprints after restoration

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/weapon_scs_detailed_output.txt"

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

    section("WEAPON SCS DETAILED CHECK")

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

        # Access Blueprint properties directly
        try:
            # Get all root nodes from SCS
            scs_nodes_prop = bp.get_editor_property('scs_nodes') if hasattr(bp, 'scs_nodes') else None
            log(f"  scs_nodes property: {scs_nodes_prop}")
        except Exception as e:
            log(f"  scs_nodes error: {e}")

        # Try using BlueprintEditorLibrary
        try:
            from unreal import BlueprintEditorLibrary
            # Get all component templates
            log(f"  Trying BlueprintEditorLibrary...")
        except:
            log(f"  BlueprintEditorLibrary not available")

        # Get generated class and check inheritance
        try:
            gen_class = bp.generated_class()
            if gen_class:
                log(f"  GeneratedClass: {gen_class.get_name()}")

                # Get CDO
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    # Check RootComponent
                    try:
                        root = cdo.get_editor_property("root_component")
                        if root:
                            log(f"  RootComponent: {root.get_name()} ({root.get_class().get_name()})")
                        else:
                            log(f"  RootComponent: None")
                    except:
                        pass

                    # Check for any components at all
                    try:
                        all_comps = cdo.get_components_by_class(unreal.ActorComponent)
                        log(f"  ActorComponents on CDO: {len(all_comps)}")
                        for comp in all_comps:
                            log(f"    - {comp.get_name()} ({comp.get_class().get_name()})")
                    except Exception as e:
                        log(f"  Component enumeration error: {e}")
        except Exception as e:
            log(f"  Class/CDO error: {e}")

    # =========================================================================
    # CHECK B_Item_Weapon SPECIFICALLY
    # =========================================================================
    section("B_Item_Weapon DETAILED CHECK")

    bp = unreal.EditorAssetLibrary.load_asset(
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon"
    )

    if bp:
        log(f"Blueprint: {bp.get_name()}")

        # List all properties
        try:
            # Blueprint type
            bp_type = type(bp).__name__
            log(f"  Type: {bp_type}")

            # Check if it's been modified
            is_dirty = bp.get_editor_property("bDirty") if hasattr(bp, "bDirty") else "N/A"
            log(f"  Is Dirty: {is_dirty}")

            # Check Blueprint status
            try:
                status = bp.get_editor_property("status")
                log(f"  Status: {status}")
            except:
                pass

        except Exception as e:
            log(f"  Property error: {e}")

        # Spawn a temporary actor to check its components
        log(f"\n  Spawning temporary actor to check components...")
        try:
            gen_class = bp.generated_class()
            if gen_class:
                # Get the editor world
                subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
                world = subsystem.get_editor_world()

                if world:
                    # Spawn at origin
                    spawn_params = unreal.SpawnActorParameters()
                    spawn_params.spawn_collision_handling_override = unreal.SpawnActorCollisionHandlingMethod.ALWAYS_SPAWN

                    transform = unreal.Transform()
                    actor = world.spawn_actor(gen_class, transform)

                    if actor:
                        log(f"  Spawned: {actor.get_name()}")

                        # Get all components
                        all_comps = actor.get_components_by_class(unreal.ActorComponent)
                        log(f"  Total Components: {len(all_comps)}")

                        for comp in all_comps:
                            comp_class = comp.get_class().get_name()
                            log(f"    - {comp.get_name()} ({comp_class})")

                            # For StaticMeshComponent, check mesh
                            if "StaticMesh" in comp_class:
                                try:
                                    mesh = comp.get_editor_property("static_mesh")
                                    if mesh:
                                        log(f"        Mesh: {mesh.get_name()}")
                                    else:
                                        log(f"        Mesh: None (NOT SET!)")
                                except:
                                    pass

                        # Destroy the temporary actor
                        actor.destroy_actor()
                        log(f"  Destroyed temporary actor")
                    else:
                        log(f"  Failed to spawn actor")
                else:
                    log(f"  No editor world available")
        except Exception as e:
            log(f"  Spawn error: {e}")

    # Write output
    output = "\n".join(results)
    with open(OUTPUT_FILE, 'w') as f:
        f.write(output)

run()
