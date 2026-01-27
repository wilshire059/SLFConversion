"""
Check B_Destructible's actual parent class to ensure it's reparented to C++.
"""

import unreal

def check_parent():
    """Check the parent class of B_Destructible."""

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible"

    output_lines = []
    def log(msg):
        output_lines.append(msg)
        print(msg)

    log("\n" + "="*80)
    log("B_Destructible Parent Class Check")
    log("="*80)

    # Load the Blueprint
    bp_asset = unreal.load_asset(bp_path)
    if not bp_asset:
        log(f"ERROR: Could not load Blueprint: {bp_path}")
        return

    log(f"\nBlueprint: {bp_asset.get_name()}")

    # Get generated class
    gen_class = bp_asset.generated_class()
    if gen_class:
        log(f"Generated Class: {gen_class.get_name()}")
        log(f"Generated Class Path: {gen_class.get_path_name()}")

        # Get parent class via get_class_default_object and walking up hierarchy
        # Or just examine the parent class directly
        parent_path = None
        try:
            # Try to get parent class via Blueprint
            parent_class = bp_asset.get_editor_property("parent_class")
            if parent_class:
                parent_path = parent_class.get_path_name()
                log(f"\nParent Class (from Blueprint): {parent_class.get_name()}")
                log(f"Parent Class Path: {parent_path}")

                # Check if it's our C++ class
                is_cpp = "/Script/SLFConversion" in str(parent_path)
                log(f"Is C++ parent: {is_cpp}")

                if not is_cpp:
                    log(f"\n*** WARNING: Blueprint is NOT reparented to C++ class! ***")
                    log(f"Expected parent path to contain: /Script/SLFConversion.B_Destructible")
                    log(f"Actual parent: {parent_path}")
                else:
                    log(f"\n*** GOOD: Blueprint is correctly reparented to C++ class ***")
        except Exception as e:
            log(f"Parent class error: {e}")

    # Also check by spawning and seeing what class we get
    log(f"\n--- Spawn Test ---")
    try:
        spawn_location = unreal.Vector(0.0, 0.0, 1000.0)
        spawned = unreal.EditorLevelLibrary.spawn_actor_from_class(gen_class, spawn_location)
        if spawned:
            actor_class = spawned.get_class()
            log(f"Spawned actor class: {actor_class.get_name()}")
            log(f"Spawned actor class path: {actor_class.get_path_name()}")

            # Check the actual C++ type
            if hasattr(spawned, 'geometry_collection'):
                gc = spawned.get_editor_property('geometry_collection')
                log(f"Has geometry_collection property: True (value: {gc.get_path_name() if gc else 'None'})")
            else:
                log(f"Has geometry_collection property: False (C++ property not found)")

            # Cleanup
            unreal.EditorLevelLibrary.destroy_actor(spawned)
    except Exception as e:
        log(f"Spawn test error: {e}")

    log("\n" + "="*80)
    log("Check Complete")
    log("="*80 + "\n")

    # Write to file
    output_path = "C:/scripts/SLFConversion/migration_cache/destructible_parent_check.txt"
    with open(output_path, "w") as f:
        f.write("\n".join(output_lines))
    log(f"Output written to: {output_path}")

if __name__ == "__main__":
    check_parent()
