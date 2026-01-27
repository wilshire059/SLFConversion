"""
Direct fix for B_BossDoor - bypass cascade loading issues.
Uses SLFAutomationLibrary functions that suppress compilation errors.
"""
import unreal

BOSS_DOOR_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"
TARGET_CLASS_PATH = "/Script/SLFConversion.SLFBossDoor"

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("DIRECT B_BOSS DOOR FIX")
    unreal.log_warning("=" * 60)

    # Load Blueprint - this may trigger compile errors but should still load
    unreal.log_warning(f"Loading {BOSS_DOOR_PATH}...")
    bp = unreal.load_asset(BOSS_DOOR_PATH)
    if not bp:
        unreal.log_error(f"Failed to load {BOSS_DOOR_PATH}")
        return

    unreal.log_warning(f"Loaded: {bp.get_name()}")

    # Step 1: Clear ALL logic using C++ function (suppresses errors)
    unreal.log_warning("Step 1: Clearing all Blueprint logic...")
    try:
        unreal.SLFAutomationLibrary.clear_all_blueprint_logic_no_compile(bp)
        unreal.log_warning("  Logic cleared")
    except Exception as e:
        unreal.log_error(f"  Clear error: {e}")
        return

    # Step 2: Remove interfaces
    unreal.log_warning("Step 2: Removing interfaces...")
    try:
        unreal.SLFAutomationLibrary.remove_implemented_interfaces(bp)
        unreal.log_warning("  Interfaces removed")
    except Exception as e:
        unreal.log_warning(f"  Interface removal error (non-fatal): {e}")

    # Step 3: Reparent to C++ class
    unreal.log_warning(f"Step 3: Reparenting to {TARGET_CLASS_PATH}...")
    try:
        ok = unreal.SLFAutomationLibrary.reparent_blueprint(bp, TARGET_CLASS_PATH)
        if ok:
            unreal.log_warning("  Reparent succeeded")
        else:
            unreal.log_error("  Reparent returned False")
            return
    except Exception as e:
        unreal.log_error(f"  Reparent error: {e}")
        return

    # Step 4: Compile the Blueprint
    unreal.log_warning("Step 4: Compiling Blueprint...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        unreal.log_warning("  Compiled")
    except Exception as e:
        unreal.log_error(f"  Compile error: {e}")

    # Step 5: Save the asset
    unreal.log_warning("Step 5: Saving...")
    try:
        unreal.EditorAssetLibrary.save_asset(BOSS_DOOR_PATH, only_if_is_dirty=False)
        unreal.log_warning("  Saved")
    except Exception as e:
        unreal.log_error(f"  Save error: {e}")

    # Verification
    unreal.log_warning("")
    unreal.log_warning("=== VERIFICATION ===")
    bp = unreal.load_asset(BOSS_DOOR_PATH)
    if bp:
        gen = bp.generated_class()
        if gen:
            cdo = unreal.get_default_object(gen)
            if cdo:
                # Check for components
                mesh_comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
                unreal.log_warning(f"StaticMeshComponents: {len(mesh_comps)}")
                for c in mesh_comps:
                    scale = c.relative_scale3d
                    loc = c.relative_location
                    unreal.log_warning(f"  {c.get_name()}: Scale ({scale.x:.2f}, {scale.y:.2f}, {scale.z:.2f})")
                    unreal.log_warning(f"    Location: ({loc.x:.2f}, {loc.y:.2f}, {loc.z:.2f})")

    unreal.log_warning("")
    unreal.log_warning("=" * 60)
    unreal.log_warning("DONE - Test in PIE")
    unreal.log_warning("=" * 60)

if __name__ == "__main__":
    main()
