# verify_container_settings.py
# Verify and update container settings including OpenDelayTime
# Ensures animation timing matches when character reaches into chest

import unreal

LEVEL_PATH = "/Game/SoulslikeFramework/Maps/L_Demo_Showcase"
EXPECTED_OPEN_DELAY = 1.2  # Seconds - matches when character hands reach chest


def find_containers_in_level():
    """Find all B_Container instances in the current level."""
    containers = []
    editor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    all_actors = editor_subsystem.get_all_level_actors()

    for actor in all_actors:
        class_name = actor.get_class().get_name()
        if "B_Container" in class_name:
            containers.append(actor)

    containers.sort(key=lambda a: a.get_actor_location().x)
    return containers


def verify_and_fix_container(container, index):
    """Verify container settings and fix if needed."""
    name = container.get_name()
    location = container.get_actor_location()
    needs_save = False

    unreal.log_warning(f"\n[Container {index}] {name} at ({location.x:.1f}, {location.y:.1f}, {location.z:.1f})")

    # Check OpenDelayTime
    try:
        open_delay = container.get_editor_property("OpenDelayTime")
        unreal.log_warning(f"  OpenDelayTime: {open_delay}")

        if abs(open_delay - EXPECTED_OPEN_DELAY) > 0.01:
            unreal.log_warning(f"  -> Updating to {EXPECTED_OPEN_DELAY}")
            container.set_editor_property("OpenDelayTime", EXPECTED_OPEN_DELAY)
            needs_save = True
        else:
            unreal.log_warning(f"  -> OK")
    except Exception as e:
        unreal.log_warning(f"  OpenDelayTime: <property not found or error: {e}>")

    # Check LidOpenAngle
    try:
        lid_angle = container.get_editor_property("LidOpenAngle")
        unreal.log_warning(f"  LidOpenAngle: {lid_angle}")
    except:
        pass

    return needs_save


def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("VERIFY CONTAINER SETTINGS")
    unreal.log_warning("=" * 70)

    # Load level
    unreal.log_warning(f"\nLoading level: {LEVEL_PATH}")
    success = unreal.EditorLoadingAndSavingUtils.load_map(LEVEL_PATH)
    if not success:
        unreal.log_error("Failed to load level")
        return

    # Find containers
    containers = find_containers_in_level()
    unreal.log_warning(f"Found {len(containers)} containers")

    # Verify each container
    any_changes = False
    for i, container in enumerate(containers):
        if verify_and_fix_container(container, i):
            any_changes = True

    # Save if changes made
    if any_changes:
        unreal.log_warning("\nSaving level with updated settings...")
        unreal.EditorLoadingAndSavingUtils.save_current_level()
        unreal.log_warning("Level saved")
    else:
        unreal.log_warning("\nNo changes needed")

    unreal.log_warning("=" * 70)


if __name__ == "__main__":
    main()
