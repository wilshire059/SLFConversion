"""
Extract BehaviorTree from Blueprint SCS (SimpleConstructionScript)
The BehaviorManager component is added via Blueprint editor, not C++ native
Run this on bp_only project!
"""
import unreal

def log(msg):
    unreal.log_warning(f"[EXTRACT_BT] {msg}")

def check_blueprint_scs(path):
    bp = unreal.load_asset(path)
    if not bp:
        log(f"Could not load: {path}")
        return

    log(f"")
    log(f"=== {bp.get_name()} ===")

    # Get SCS nodes using automation library
    scs_components = unreal.SLFAutomationLibrary.get_blueprint_scs_components(bp)
    log(f"  SCS Components: {scs_components}")

    # Also try to get variables which might include the BT reference
    variables = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
    log(f"  Variables: {variables}")

    # Get parent class
    parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(bp)
    log(f"  Parent: {parent}")

def main():
    log("=" * 70)
    log("EXTRACTING ENEMY SCS INFO FROM BACKUP")
    log("=" * 70)

    check_blueprint_scs("/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy")
    check_blueprint_scs("/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Enemy_Guard")

    log("")
    log("=" * 70)

if __name__ == "__main__":
    main()
