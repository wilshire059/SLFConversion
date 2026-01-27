"""
Test loading a single BT subtree to diagnose hang
"""
import unreal

def main():
    unreal.log_warning("[TEST] Starting BT load test")

    bt_paths = [
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Combat",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_Idle",
        "/Game/SoulslikeFramework/Blueprints/_AI/Subtrees/BT_RandomRoam",
    ]

    for bt_path in bt_paths:
        unreal.log_warning(f"[TEST] Loading: {bt_path}")
        try:
            bt = unreal.load_asset(bt_path)
            if bt:
                unreal.log_warning(f"[TEST] OK: {bt.get_name()}")
            else:
                unreal.log_warning(f"[TEST] FAIL: Could not load")
        except Exception as e:
            unreal.log_warning(f"[TEST] ERROR: {e}")

    unreal.log_warning("[TEST] Done")

if __name__ == "__main__":
    main()
