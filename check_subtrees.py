"""
Check which subtrees contain ToggleFocus task
"""
import unreal

def log(msg):
    unreal.log_warning(f"[SUBTREE] {msg}")

def main():
    log("=" * 70)
    log("CHECKING SUBTREES FOR TOGGLEFOCUS")
    log("=" * 70)
    
    subtrees = [
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Idle",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Investigating", 
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_RandomRoam",
    ]
    
    for path in subtrees:
        bt = unreal.load_asset(path)
        if bt:
            # Export text to search for ToggleFocus
            try:
                export = bt.export_text()
                if "ToggleFocus" in export:
                    log(f"{bt.get_name()}: CONTAINS ToggleFocus")
                else:
                    log(f"{bt.get_name()}: No ToggleFocus")
            except:
                log(f"{bt.get_name()}: Could not export")
        else:
            log(f"{path}: FAILED TO LOAD")
    
    log("=" * 70)

if __name__ == "__main__":
    main()
