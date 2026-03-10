import unreal

def log(msg):
    print(f"[FixFirstTime] {msg}")
    unreal.log_warning(f"[FixFirstTime] {msg}")

# Correct path
gi_path = "/Game/SoulslikeFramework/Blueprints/Global/GI_SoulslikeFramework"
log(f"Loading: {gi_path}")

gi_bp = unreal.EditorAssetLibrary.load_asset(gi_path)
if gi_bp:
    log(f"Loaded: {gi_bp.get_name()}")
    gen_class = gi_bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            # Check current value
            try:
                current = cdo.get_editor_property('first_time_on_demo_level')
                log(f"Current FirstTimeOnDemoLevel: {current}")
            except Exception as e:
                log(f"Error getting property: {e}")
            
            # Set to True
            try:
                cdo.set_editor_property('first_time_on_demo_level', True)
                log("Set FirstTimeOnDemoLevel = True")
            except Exception as e:
                log(f"Error setting property: {e}")
            
            # Verify
            try:
                new_val = cdo.get_editor_property('first_time_on_demo_level')
                log(f"New FirstTimeOnDemoLevel: {new_val}")
            except Exception as e:
                log(f"Error verifying: {e}")
    
    # Save
    result = unreal.EditorAssetLibrary.save_asset(gi_path)
    log(f"Save result: {result}")
else:
    log("Could not load GameInstance Blueprint")
