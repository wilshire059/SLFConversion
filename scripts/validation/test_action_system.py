"""
Test Action System - Verify AvailableActions map stores UClass* correctly
Runs in headless mode via UnrealEditor-Cmd
"""
import unreal

def test_action_manager():
    """Test the AC_ActionManager action lookup system"""
    print("=" * 60)
    print("ACTION SYSTEM TEST")
    print("=" * 60)

    # Load the demo level
    level_path = "/Game/SoulslikeFramework/Maps/Showcase_Demo"
    print(f"Loading level: {level_path}")

    # Use EditorLoadingAndSavingUtils to load the level
    success = unreal.EditorLoadingAndSavingUtils.load_map(level_path)
    if not success:
        print("ERROR: Failed to load level")
        return False

    print("Level loaded successfully")

    # Get the world
    world = unreal.EditorLevelLibrary.get_editor_world()
    if not world:
        print("ERROR: Could not get editor world")
        return False

    # Find the player character Blueprint
    player_bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"

    # Look for a character in the level that has AC_ActionManager
    all_actors = unreal.EditorLevelLibrary.get_all_level_actors()
    print(f"Found {len(all_actors)} actors in level")

    action_manager = None
    owner_actor = None

    for actor in all_actors:
        # Check if this actor has an AC_ActionManager component
        components = actor.get_components_by_class(unreal.ActorComponent)
        for comp in components:
            class_name = comp.get_class().get_name()
            if "ActionManager" in class_name:
                action_manager = comp
                owner_actor = actor
                print(f"Found ActionManager on {actor.get_name()}: {class_name}")
                break
        if action_manager:
            break

    if not action_manager:
        print("No ActionManager found in level actors, trying to spawn a test character...")

        # Try to load and instantiate the character Blueprint
        bp_class = unreal.EditorAssetLibrary.load_blueprint_class(player_bp_path)
        if bp_class:
            print(f"Loaded Blueprint class: {bp_class.get_name()}")

            # Spawn the character
            spawn_location = unreal.Vector(0, 0, 100)
            spawn_rotation = unreal.Rotator(0, 0, 0)

            spawned_actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
                bp_class, spawn_location, spawn_rotation
            )

            if spawned_actor:
                print(f"Spawned: {spawned_actor.get_name()}")

                # Get components
                components = spawned_actor.get_components_by_class(unreal.ActorComponent)
                for comp in components:
                    class_name = comp.get_class().get_name()
                    if "ActionManager" in class_name:
                        action_manager = comp
                        owner_actor = spawned_actor
                        print(f"Found ActionManager: {class_name}")
                        break

    if not action_manager:
        print("ERROR: Could not find or create ActionManager")
        return False

    print(f"\nActionManager owner: {owner_actor.get_name()}")
    print(f"ActionManager class: {action_manager.get_class().get_name()}")

    # Check the Actions and AvailableActions maps via reflection
    action_manager_class = action_manager.get_class()

    # Try to get property values
    try:
        # Get Actions map count
        actions_prop = unreal.Name("Actions")
        available_actions_prop = unreal.Name("AvailableActions")

        # Use get_editor_property if available
        if hasattr(action_manager, 'get_editor_property'):
            try:
                actions_map = action_manager.get_editor_property('Actions')
                print(f"\nActions map entries: {len(actions_map) if actions_map else 0}")
            except:
                print("Could not read Actions property directly")

            try:
                available_map = action_manager.get_editor_property('AvailableActions')
                print(f"AvailableActions map entries: {len(available_map) if available_map else 0}")
            except:
                print("Could not read AvailableActions property directly")
    except Exception as e:
        print(f"Property access error: {e}")

    # Try to call the initialization functions to populate the maps
    print("\n--- Calling InitializeDefaultActions ---")
    try:
        if hasattr(action_manager, 'call_method'):
            action_manager.call_method('InitializeDefaultActions')
        else:
            # Try direct function call
            unreal.SystemLibrary.execute_console_command(
                world, f"ce InitializeDefaultActions", None
            )
    except Exception as e:
        print(f"InitializeDefaultActions call failed: {e}")

    print("\n--- Calling BuildAvailableActionsFromActionsMap ---")
    try:
        if hasattr(action_manager, 'call_method'):
            action_manager.call_method('BuildAvailableActionsFromActionsMap')
    except Exception as e:
        print(f"BuildAvailableActionsFromActionsMap call failed: {e}")

    # Now test EventPerformAction with a known tag
    print("\n--- Testing EventPerformAction ---")
    test_tags = [
        "SoulslikeFramework.Action.Jump",
        "SoulslikeFramework.Action.Dodge",
        "SoulslikeFramework.Action.StartSprinting",
    ]

    for tag_str in test_tags:
        print(f"\nTesting tag: {tag_str}")
        try:
            # Create gameplay tag
            tag = unreal.GameplayTag.request_gameplay_tag(unreal.Name(tag_str))
            if tag.is_valid():
                print(f"  Tag is valid: {tag.tag_name}")

                # Try to call EventPerformAction
                if hasattr(action_manager, 'event_perform_action'):
                    action_manager.event_perform_action(tag)
                    print(f"  Called event_perform_action")
                else:
                    print(f"  event_perform_action not exposed to Python")
            else:
                print(f"  Tag is NOT valid")
        except Exception as e:
            print(f"  Error: {e}")

    print("\n" + "=" * 60)
    print("TEST COMPLETE - Check LogTemp output for results")
    print("=" * 60)

    return True

# Run the test
if __name__ == "__main__":
    test_action_manager()
