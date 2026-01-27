import unreal

# Map Blueprint notify paths to C++ parent classes
NOTIFY_REPARENT_MAP = {
    "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_InputBuffer":
        "/Script/SLFConversion.ANS_InputBuffer",
    "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_RegisterAttackSequence":
        "/Script/SLFConversion.ANS_RegisterAttackSequence",
}

el = unreal.EditorAssetLibrary
bel = unreal.BlueprintEditorLibrary

for bp_path, cpp_class_path in NOTIFY_REPARENT_MAP.items():
    print(f"\n=== Processing {bp_path.split('/')[-1]} ===")

    # Load the Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        print(f"  ERROR: Could not load Blueprint")
        continue

    # Load the C++ parent class
    cpp_class = unreal.load_class(None, cpp_class_path)
    if not cpp_class:
        print(f"  ERROR: Could not load C++ class {cpp_class_path}")
        continue

    print(f"  Blueprint loaded: {bp.get_name()}")
    print(f"  Target C++ class: {cpp_class.get_name()}")

    # Reparent
    try:
        result = bel.reparent_blueprint(bp, cpp_class)
        if result:
            print(f"  SUCCESS: Reparented to {cpp_class.get_name()}")
            el.save_asset(bp_path)
            print(f"  Saved.")
        else:
            print(f"  WARNING: Reparent returned False (may already have correct parent)")
    except Exception as e:
        print(f"  ERROR during reparent: {e}")

print("\n=== Reparenting complete ===")
