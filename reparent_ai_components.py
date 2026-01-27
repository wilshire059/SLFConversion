# reparent_ai_components.py
# Reparent AI component Blueprints to their C++ base classes

import unreal

# Mapping of Blueprint components to their C++ parent classes
COMPONENT_MAP = {
    "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_BehaviorManager": "AIBehaviorManagerComponent",
    "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_CombatManager": "AICombatManagerComponent",
}

def reparent_blueprint(bp_path, cpp_class_name):
    """Reparent a Blueprint to a C++ class"""
    print(f"\nProcessing: {bp_path}")

    # Load the Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        print(f"  ERROR: Could not load Blueprint")
        return False

    # Load the C++ class
    cpp_class = unreal.load_class(None, f"/Script/SLFConversion.{cpp_class_name}")
    if not cpp_class:
        print(f"  ERROR: Could not load C++ class {cpp_class_name}")
        return False

    print(f"  Blueprint: {bp.get_name()}")
    print(f"  Target C++ class: {cpp_class_name}")

    # Get current parent
    gen = bp.generated_class()
    if gen:
        try:
            parent = gen.get_super_class()
            if parent:
                print(f"  Current parent: {parent.get_name()}")
                if parent.get_name() == cpp_class_name or parent.get_name() == f"U{cpp_class_name}":
                    print(f"  Already reparented to {cpp_class_name}, skipping")
                    return True
        except:
            pass

    # Reparent using BlueprintEditorLibrary
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        if result:
            print(f"  REPARENTED to {cpp_class_name}")

            # Save the asset
            unreal.EditorAssetLibrary.save_asset(bp_path)
            print(f"  SAVED")
            return True
        else:
            print(f"  FAILED to reparent")
            return False
    except Exception as e:
        print(f"  Exception: {e}")
        return False

def main():
    print("=" * 70)
    print("REPARENTING AI COMPONENT BLUEPRINTS TO C++")
    print("=" * 70)

    success_count = 0
    fail_count = 0

    for bp_path, cpp_class in COMPONENT_MAP.items():
        if reparent_blueprint(bp_path, cpp_class):
            success_count += 1
        else:
            fail_count += 1

    print("\n" + "=" * 70)
    print(f"RESULTS: {success_count} succeeded, {fail_count} failed")
    print("=" * 70)

if __name__ == "__main__":
    main()
