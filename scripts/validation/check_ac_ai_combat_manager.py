import unreal

def log(msg):
    print(f"[CheckAC] {msg}")
    unreal.log_warning(f"[CheckAC] {msg}")

# Check the AC_AI_CombatManager Blueprint component
ac_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_AI_CombatManager"

log(f"Loading: {ac_path}")
ac_bp = unreal.EditorAssetLibrary.load_asset(ac_path)

if not ac_bp:
    log("ERROR: Could not load AC_AI_CombatManager")
else:
    log(f"Loaded: {ac_bp.get_name()}")
    log(f"Class: {ac_bp.get_class().get_name()}")
    
    # Get parent class
    try:
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(ac_bp)
        log(f"Parent class: {parent}")
    except Exception as e:
        log(f"Error getting parent: {e}")
    
    # Check for PoiseBreakAsset property
    gen_class = ac_bp.generated_class()
    if gen_class:
        log(f"Generated class: {gen_class.get_name()}")
        
        # Check if it has PoiseBreakAsset
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            try:
                pba = cdo.get_editor_property('poise_break_asset')
                log(f"PoiseBreakAsset property exists: {pba}")
            except Exception as e:
                log(f"PoiseBreakAsset property error: {e}")

log("")

# Also check the C++ class
log("Checking C++ UAC_AI_CombatManager class...")
try:
    cpp_class = unreal.load_class(None, "/Script/SLFConversion.AC_AI_CombatManager")
    if cpp_class:
        log(f"C++ class exists: {cpp_class.get_name()}")
    else:
        log("C++ class not found")
except Exception as e:
    log(f"C++ class error: {e}")

log("")
log("=" * 70)
log("DONE")
