"""Check AIC_SoulslikeFramework Blueprint parent class"""
import unreal

def log(msg):
    unreal.log_warning(f"[CHECK_AIC] {msg}")

def main():
    bp_path = "/Game/SoulslikeFramework/Blueprints/_AI/Misc/AIC_SoulslikeFramework"
    bp = unreal.load_asset(bp_path)

    if not bp:
        log(f"Could not load: {bp_path}")
        return

    log(f"Blueprint: {bp.get_name()}")

    # Use the automation library to get parent
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"Parent class path: {parent}")

if __name__ == "__main__":
    main()
