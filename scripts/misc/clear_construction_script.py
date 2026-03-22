"""
Clear ConstructionScript from B_DemoRoom so C++ OnConstruction runs instead
Uses SLFAutomationLibrary.ClearConstructionScript() function
"""
import unreal

eal = unreal.EditorAssetLibrary

def log(msg):
    unreal.log_warning(msg)

def clear_construction_script(bp_path):
    """Clear the ConstructionScript from a Blueprint using SLFAutomationLibrary"""
    log(f"=== Clearing ConstructionScript from {bp_path} ===")
    bp = eal.load_asset(bp_path)

    if not bp:
        log(f"  ERROR: Could not load {bp_path}")
        return False

    log(f"  Blueprint class: {bp.get_class().get_name()}")

    # Use the new ClearConstructionScript function from SLFAutomationLibrary
    try:
        result = unreal.SLFAutomationLibrary.clear_construction_script(bp)
        log(f"  ClearConstructionScript result: {result}")
    except Exception as e:
        log(f"  ClearConstructionScript failed: {e}")
        return False

    # Save the Blueprint
    try:
        eal.save_asset(bp_path, only_if_is_dirty=False)
        log(f"  Saved {bp_path}")
        return True
    except Exception as e:
        log(f"  Save error: {e}")
        return False

# Clear B_DemoRoom's ConstructionScript
bp_path = "/Game/SoulslikeFramework/Demo/ShowcaseRoom/DemoRoom/Blueprints/B_DemoRoom"
clear_construction_script(bp_path)

log("=== Done! ===")
