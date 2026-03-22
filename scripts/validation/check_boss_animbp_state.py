import unreal

def log(msg):
    print(f"[CheckBossABP] {msg}")
    unreal.log_warning(f"[CheckBossABP] {msg}")

abp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew"

log(f"Loading: {abp_path}")
abp = unreal.EditorAssetLibrary.load_asset(abp_path)

if not abp:
    log("ERROR: Could not load ABP_SoulslikeBossNew")
else:
    log(f"Loaded: {abp.get_name()}")
    
    # Check parent class
    try:
        parent = unreal.SLFAutomationLibrary.get_blueprint_parent_class(abp)
        log(f"Parent class: {parent}")
    except Exception as e:
        log(f"Error getting parent: {e}")
    
    # Check if has event graph logic
    try:
        has_logic = unreal.SLFAutomationLibrary.has_event_graph_logic(abp)
        log(f"Has EventGraph logic: {has_logic}")
    except Exception as e:
        log(f"Error checking EventGraph: {e}")
    
    # Check compile errors
    try:
        errors = unreal.SLFAutomationLibrary.get_blueprint_compile_errors(abp)
        log(f"Compile errors: {errors if errors else 'None'}")
    except Exception as e:
        log(f"Error checking compile: {e}")
    
    # Try to get all graphs
    try:
        graphs = unreal.BlueprintEditorLibrary.get_blueprint_graphs(abp)
        log(f"Number of graphs: {len(graphs) if graphs else 0}")
        if graphs:
            for i, g in enumerate(graphs):
                log(f"  Graph {i}: {g.get_name() if g else 'None'}")
    except Exception as e:
        log(f"Error getting graphs: {e}")
