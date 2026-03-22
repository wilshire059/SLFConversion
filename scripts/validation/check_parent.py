import unreal

eal = unreal.EditorAssetLibrary

def log(msg):
    unreal.log_warning(msg)

bp_path = "/Game/SoulslikeFramework/Demo/ShowcaseRoom/DemoRoom/Blueprints/B_DemoRoom"
bp = eal.load_asset(bp_path)

if bp:
    log(f"Blueprint loaded: {bp.get_name()}")
    log(f"Blueprint class: {bp.get_class().get_name()}")
    
    # Get parent class
    try:
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
        log(f"Parent class: {parent}")
    except Exception as e:
        log(f"Error getting parent: {e}")
else:
    log("Could not load Blueprint")

log("=== Done ===")
