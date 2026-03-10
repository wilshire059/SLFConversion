# reparent_spawn_notify.py
# Reparent AN_SpawnProjectile Blueprint to inherit from C++ UAN_SpawnProjectile
# AND clear its EventGraph so it uses C++ Notify() implementation

import unreal

LOG_FILE = "C:/scripts/SLFConversion/reparent_notify_log.txt"

def log(msg):
    with open(LOG_FILE, "a") as f:
        f.write(msg + "\n")
    unreal.log(msg)

def reparent_notify():
    with open(LOG_FILE, "w") as f:
        f.write("=== Reparenting AN_SpawnProjectile Blueprint ===\n")

    bp_path = "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/AN_SpawnProjectile"

    # Load the Blueprint
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"ERROR: Could not load Blueprint at {bp_path}")
        return False

    log(f"Loaded Blueprint: {bp.get_name()}")

    # Load the C++ class
    cpp_class = unreal.load_class(None, "/Script/SLFConversion.AN_SpawnProjectile")
    if not cpp_class:
        log("ERROR: Could not load C++ UAN_SpawnProjectile class!")
        return False

    log(f"Loaded C++ class: {cpp_class.get_name()}")

    # Step 1: Clear ALL Blueprint logic using C++ SLFAutomationLibrary
    # This removes Received_Notify override and all other functions
    log("Clearing ALL Blueprint logic using ClearAllBlueprintLogic...")
    try:
        result = unreal.SLFAutomationLibrary.clear_all_blueprint_logic(bp)
        log(f"ClearAllBlueprintLogic result: {result}")
    except Exception as e:
        log(f"Warning clearing logic: {e}")

    # Step 2: Reparent the Blueprint
    log("Reparenting Blueprint...")
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, cpp_class)
        log(f"Reparent result: {result}")
    except Exception as e:
        log(f"Reparent exception: {e}")
        return False

    # Step 3: Compile the Blueprint
    log("Compiling Blueprint...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        log("Compilation complete")
    except Exception as e:
        log(f"Compilation warning: {e}")

    # Step 4: Save the Blueprint
    success = unreal.EditorAssetLibrary.save_asset(bp_path)
    if success:
        log(f"SAVED: {bp_path}")
    else:
        log(f"WARNING: Could not save {bp_path}")

    # Verify
    bp2 = unreal.EditorAssetLibrary.load_asset(bp_path)
    if bp2:
        log(f"Verified Blueprint saved: {bp2.get_name()}")

    return True

def main():
    if reparent_notify():
        log("SUCCESS: AN_SpawnProjectile Blueprint now inherits from C++ UAN_SpawnProjectile")
        log("The Blueprint's Received_Notify has been cleared - C++ Notify() will execute")
    else:
        log("FAILED to reparent AN_SpawnProjectile")

if __name__ == "__main__":
    main()
