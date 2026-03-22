import unreal

def log(msg):
    print(f"[CheckBoss] {msg}")
    unreal.log_warning(f"[CheckBoss] {msg}")

# Check what components the boss character has
boss_path = "/Game/SoulslikeFramework/Blueprints/_Characters/Enemies/B_Soulslike_Boss"

log(f"Loading: {boss_path}")
boss_bp = unreal.EditorAssetLibrary.load_asset(boss_path)

if not boss_bp:
    log("ERROR: Could not load B_Soulslike_Boss")
else:
    log(f"Loaded: {boss_bp.get_name()}")
    
    # Get generated class
    gen_class = boss_bp.generated_class()
    if gen_class:
        log(f"Generated class: {gen_class.get_name()}")
        
        # Get CDO and check components
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            log(f"CDO: {cdo.get_name()}")
            
            # List all components
            components = cdo.get_components_by_class(unreal.ActorComponent)
            log(f"Components ({len(components)}):")
            for comp in components:
                log(f"  - {comp.get_name()} : {comp.get_class().get_name()}")
                
            # Specifically look for combat manager
            for comp in components:
                class_name = comp.get_class().get_name()
                if "combat" in class_name.lower() or "ai" in class_name.lower():
                    log(f"  FOUND COMBAT: {comp.get_name()} -> {comp.get_class().get_full_name()}")
    
    # Check parent class
    try:
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(boss_bp)
        log(f"Parent class: {parent}")
    except Exception as e:
        log(f"Error getting parent: {e}")

log("")
log("=" * 70)
log("DONE")
