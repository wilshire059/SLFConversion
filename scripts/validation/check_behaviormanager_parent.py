# check_behaviormanager_parent.py
# Check if AC_AI_BehaviorManager is reparented to UAIBehaviorManagerComponent

import unreal

def main():
    # Load the Blueprint component
    bp_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_BehaviorManager"
    bp = unreal.load_asset(bp_path)

    if not bp:
        print(f"ERROR: Could not load {bp_path}")
        return

    print(f"Blueprint: {bp.get_name()}")
    print(f"  Class: {bp.get_class().get_name()}")

    # Get parent class
    try:
        gen_class = bp.generated_class()
        if gen_class:
            print(f"  GeneratedClass: {gen_class.get_name()}")

            # Walk parent hierarchy
            current = gen_class
            print("  Hierarchy:")
            depth = 0
            while current and depth < 10:
                parent_name = current.get_name()
                print(f"    [{depth}] {parent_name}")
                # Try to get parent class using static_class
                try:
                    current = current.static_class().get_super_class()
                except:
                    # Alternative: get outer for CDOs
                    current = None
                depth += 1
        else:
            print("  GeneratedClass: None")
    except Exception as e:
        print(f"  Error getting parent: {e}")

    # Check if we can find the C++ class
    print("\nLooking for C++ class...")
    try:
        # Try to find using find_class
        for cls_name in ["AIBehaviorManagerComponent", "UAIBehaviorManagerComponent"]:
            cpp_class = unreal.find_class(cls_name)
            if cpp_class:
                print(f"  Found: {cls_name}")
                break
    except Exception as e:
        print(f"  find_class error: {e}")

    # Check enemy to see what components it has
    print("\n--- Checking B_Soulslike_Enemy components ---")
    enemy_bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy"
    enemy_bp = unreal.load_asset(enemy_bp_path)
    if enemy_bp:
        gen = enemy_bp.generated_class()
        if gen:
            cdo = gen.get_default_object()
            if cdo:
                # Get all components
                comps = cdo.get_components_by_class(unreal.ActorComponent)
                print(f"  Default Object Components: {len(comps)}")
                for comp in comps:
                    print(f"    - {comp.get_name()} ({comp.get_class().get_name()})")

if __name__ == "__main__":
    main()
