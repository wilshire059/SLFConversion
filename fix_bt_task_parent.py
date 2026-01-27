# fix_bt_task_parent.py
# Reparent BTT_TryExecuteAbility Blueprint to C++ UBTT_TryExecuteAbility

import unreal

def log(msg):
    unreal.log_warning(f"[FIX_BT] {msg}")

def main():
    log("=" * 70)
    log("Reparenting BTT_TryExecuteAbility to C++ parent")
    log("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Blueprints/_AI/Tasks/BTT_TryExecuteAbility"
    cpp_parent_path = "/Script/SLFConversion.BTT_TryExecuteAbility"

    # Load the Blueprint
    bp = unreal.load_asset(bp_path)
    if not bp:
        log(f"ERROR: Could not load {bp_path}")
        return

    log(f"Loaded Blueprint: {bp.get_name()}")

    # Get current parent
    if hasattr(unreal, 'SLFAutomationLibrary'):
        current_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"Current parent: {current_parent}")

    # Load the C++ parent class
    cpp_class = unreal.load_class(None, cpp_parent_path)
    if not cpp_class:
        log(f"ERROR: Could not load C++ class {cpp_parent_path}")
        return

    log(f"C++ parent class: {cpp_class.get_name()}")

    # Reparent
    result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
    if result:
        log("Reparent successful!")

        # Compile and save
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
        log("Compiled and saved")

        # Verify new parent
        if hasattr(unreal, 'SLFAutomationLibrary'):
            new_parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
            log(f"New parent: {new_parent}")
    else:
        log("ERROR: Reparent failed!")

    log("=" * 70)
    log("Done!")
    log("=" * 70)

if __name__ == "__main__":
    main()
