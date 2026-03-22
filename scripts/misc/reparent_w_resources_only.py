import unreal

# Reparent W_Resources to C++ without clearing anything
# This preserves the widget tree styling

bp_path = "/Game/SoulslikeFramework/Widgets/HUD/W_Resources"
cpp_path = "/Script/SLFConversion.W_Resources"

bp = unreal.load_asset(bp_path)
if not bp:
    unreal.log_error("ERROR: Could not load " + bp_path)
else:
    unreal.log_warning("=== Reparenting W_Resources (preserve widget tree) ===")

    # Check current parent
    gen_class = bp.generated_class()
    if gen_class:
        parent = gen_class.get_super_class()
        unreal.log_warning(f"Current parent: {parent.get_name() if parent else 'None'}")

    # Reparent to C++
    ok = unreal.SLFAutomationLibrary.reparent_blueprint(bp, cpp_path)
    unreal.log_warning(f"Reparent result: {ok}")

    # Try to compile
    result = unreal.SLFAutomationLibrary.compile_and_save(bp)
    unreal.log_warning(f"Compile result: {result}")

    if result:
        unreal.log_warning("SUCCESS: W_Resources reparented and compiled!")
    else:
        unreal.log_error("FAILED: Compile failed - check for pin errors")

    unreal.log_warning("=== Done ===")
