# check_weapon_childactor.py
# Check if ChildActorComponent "Weapon" has ChildActorClass configured

import unreal

OUTPUT_FILE = "C:/scripts/SLFConversion/weapon_childactor_output.txt"

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

    section("WEAPON CHILDACTORCOMPONENT DIAGNOSTIC")

    # Enemy Blueprints with weapons
    enemy_bps = [
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Showcase",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss_Malgareth",
    ]

    for bp_path in enemy_bps:
        bp_name = bp_path.split("/")[-1]
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)

        if not bp:
            log(f"  {bp_name}: NOT FOUND")
            continue

        log(f"")
        log(f"[{bp_name}]")

        gen_class = bp.generated_class()
        if not gen_class:
            log(f"  No generated class")
            continue

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            log(f"  No CDO")
            continue

        # Get all ChildActorComponents
        try:
            child_actor_comps = cdo.get_components_by_class(unreal.ChildActorComponent)
            log(f"  ChildActorComponents: {len(child_actor_comps)}")

            for cac in child_actor_comps:
                comp_name = cac.get_name()
                log(f"")
                log(f"    [{comp_name}]")

                # Check ChildActorClass
                try:
                    child_class = cac.get_editor_property("ChildActorClass")
                    if child_class:
                        log(f"      ChildActorClass: {child_class.get_name()}")
                    else:
                        log(f"      ChildActorClass: None (NOT SET!)")
                except Exception as e:
                    log(f"      ChildActorClass error: {e}")

                # Check if there's a child actor already
                try:
                    child_actor = cac.get_child_actor()
                    if child_actor:
                        log(f"      ChildActor: {child_actor.get_name()} ({child_actor.get_class().get_name()})")
                    else:
                        log(f"      ChildActor: None")
                except:
                    pass

                # Check attachment
                try:
                    attach_parent = cac.get_attach_parent()
                    if attach_parent:
                        log(f"      AttachedTo: {attach_parent.get_name()}")
                    attach_socket = cac.get_attach_socket_name()
                    if attach_socket and attach_socket != "None":
                        log(f"      Socket: {attach_socket}")
                except:
                    pass

        except Exception as e:
            log(f"  Error getting components: {e}")

    # =========================================================================
    # CHECK WEAPON BLUEPRINTS
    # =========================================================================
    section("AI WEAPON BLUEPRINTS")

    weapon_bps = [
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_AI_Weapon",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Sword",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_Greatsword",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_BossMace",
    ]

    for wp_path in weapon_bps:
        wp_name = wp_path.split("/")[-1]
        wp = unreal.EditorAssetLibrary.load_asset(wp_path)

        if wp:
            parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(wp)
            log(f"  {wp_name}")
            log(f"    Parent: {parent}")

            # Check if it can be loaded as class
            class_path = wp_path + "." + wp_name + "_C"
            try:
                cls = unreal.load_class(None, class_path)
                if cls:
                    log(f"    Class loadable: YES")
                else:
                    log(f"    Class loadable: NO")
            except:
                log(f"    Class load error")
        else:
            log(f"  {wp_name}: NOT FOUND")

    # =========================================================================
    # CHECK LEVEL INSTANCES
    # =========================================================================
    section("LEVEL ENEMY INSTANCES")

    level_path = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
    try:
        unreal.EditorLevelLibrary.load_level(level_path)
        subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
        world = subsystem.get_editor_world()

        if world:
            characters = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Character)

            for char in characters:
                char_name = char.get_name()
                if "Enemy" not in char_name and "Boss" not in char_name:
                    continue

                log(f"")
                log(f"[{char_name}]")
                log(f"  Class: {char.get_class().get_name()}")

                # Get ChildActorComponents from instance
                try:
                    child_actor_comps = char.get_components_by_class(unreal.ChildActorComponent)
                    log(f"  ChildActorComponents: {len(child_actor_comps)}")

                    for cac in child_actor_comps:
                        comp_name = cac.get_name()
                        log(f"    - {comp_name}")

                        # Check ChildActorClass
                        try:
                            child_class = cac.get_editor_property("ChildActorClass")
                            if child_class:
                                log(f"        ChildActorClass: {child_class.get_name()}")
                            else:
                                log(f"        ChildActorClass: None (NOT SET!)")
                        except Exception as e:
                            log(f"        ChildActorClass error: {e}")

                        # Check child actor
                        try:
                            child = cac.get_child_actor()
                            if child:
                                log(f"        ChildActor: {child.get_name()}")
                            else:
                                log(f"        ChildActor: None (NO WEAPON!)")
                        except:
                            pass

                except Exception as e:
                    log(f"  Component error: {e}")

    except Exception as e:
        log(f"  Level error: {e}")

    # =========================================================================
    # DIAGNOSIS
    # =========================================================================
    section("DIAGNOSIS")

    log("")
    log("If ChildActorClass is None, the weapon class reference was likely")
    log("cleared during the migration process when Blueprint logic was removed.")
    log("")
    log("FIX: Set the ChildActorClass on each enemy's Weapon component:")
    log("  - B_Soulslike_Enemy_Guard -> B_Item_AI_Weapon_Sword")
    log("  - B_Soulslike_Boss_Malgareth -> B_Item_AI_Weapon_BossMace")
    log("")
    log("This can be done via Python script or in the Blueprint editor.")

    # Write output
    output = "\n".join(results)
    with open(OUTPUT_FILE, 'w') as f:
        f.write(output)

run()
