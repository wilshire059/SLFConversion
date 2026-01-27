# check_animbp_state.py
# Comprehensive check of AnimBP state

import unreal

ANIMBP_PATHS = {
    "Player": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
    "Enemy": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeEnemy",
    "Boss": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew",
    "NPC": "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeNPC",
}

def log(msg):
    """Log using unreal.log for visibility"""
    unreal.log_warning(msg)

def check_animbp(name, path):
    log(f"=== {name}: {path.split('/')[-1]} ===")
    
    bp = unreal.load_asset(path)
    if not bp:
        log(f"  [ERROR] Cannot load asset")
        return
    
    # Check parent class via parent_class property
    try:
        parent_class = bp.get_editor_property('parent_class')
        if parent_class:
            parent_name = parent_class.get_name()
            parent_path = parent_class.get_path_name()
            log(f"  Parent Class: {parent_name}")
            log(f"  Parent Path: {parent_path}")
            if "/Script/" in parent_path:
                log(f"  [OK] Parent is C++ class")
            else:
                log(f"  [WARN] Parent is Blueprint class - NativeUpdateAnimation won't run!")
        else:
            log(f"  Parent Class: None")
    except Exception as e:
        log(f"  [ERROR] Cannot get parent_class: {e}")
    
    # Check for EventGraph content
    try:
        uber_graphs = bp.get_editor_property('uber_graph_pages')
        if uber_graphs:
            total_nodes = 0
            for graph in uber_graphs:
                if graph:
                    try:
                        nodes = graph.get_editor_property('nodes')
                        if nodes:
                            total_nodes += len(nodes)
                    except:
                        pass
            log(f"  EventGraph Nodes: {total_nodes}")
            if total_nodes > 0:
                log(f"  [WARN] EventGraph has {total_nodes} nodes")
        else:
            log(f"  EventGraph: None")
    except Exception as e:
        log(f"  [ERROR] Cannot check EventGraph: {e}")

def main():
    log("=" * 60)
    log("AnimBP State Check")
    log("=" * 60)
    
    for name, path in ANIMBP_PATHS.items():
        check_animbp(name, path)
    
    log("=" * 60)
    log("Check Complete")
    log("=" * 60)

if __name__ == "__main__":
    main()
