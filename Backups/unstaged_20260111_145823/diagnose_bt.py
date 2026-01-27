"""
Diagnose Behavior Tree structure - check decorators and state checks
"""
import unreal

def log(msg):
    unreal.log_warning(f"[BT_DIAG] {msg}")

def main():
    log("=" * 70)
    log("BEHAVIOR TREE DIAGNOSTIC")
    log("=" * 70)

    # Load behavior tree
    bt_path = "/Game/SoulslikeFramework/Blueprints/_AI/BT_Enemy"
    bt = unreal.load_asset(bt_path)

    if not bt:
        log(f"[ERROR] Could not load behavior tree: {bt_path}")
        return

    log(f"Loaded: {bt.get_name()}")
    log(f"Class: {bt.get_class().get_name()}")

    # Check blackboard asset
    bb = bt.get_editor_property("blackboard_asset")
    if bb:
        log(f"Blackboard: {bb.get_name()}")
        log(f"Blackboard Path: {bb.get_path_name()}")

        # Get keys
        keys = bb.get_editor_property("keys")
        log(f"Blackboard Keys ({len(keys)}):")
        for key in keys:
            key_name = key.get_editor_property("entry_name")
            key_type = key.get_editor_property("key_type")
            if key_type:
                log(f"  - {key_name}: {key_type.get_class().get_name()}")
            else:
                log(f"  - {key_name}: (no type)")
    else:
        log("[WARNING] No blackboard asset!")

    # Check root node
    root = bt.get_editor_property("root_node")
    if root:
        log(f"Root Node: {root.get_class().get_name()}")

        # Check if it has child nodes
        if hasattr(root, 'get_editor_property'):
            try:
                child = root.get_editor_property("child_node")
                if child:
                    log(f"  Child: {child.get_class().get_name()}")

                    # If it's a composite, check children
                    if hasattr(child, 'get_editor_property'):
                        try:
                            children = child.get_editor_property("children")
                            if children:
                                log(f"  Children ({len(children)}):")
                                for i, c in enumerate(children):
                                    log(f"    [{i}] {c.get_class().get_name()}")

                                    # Check decorators
                                    try:
                                        decorators = c.get_editor_property("decorators")
                                        if decorators:
                                            for d in decorators:
                                                log(f"        Decorator: {d.get_class().get_name()}")
                                                # Try to get decorator details
                                                try:
                                                    node_name = d.get_editor_property("node_name")
                                                    log(f"          Name: {node_name}")
                                                except:
                                                    pass
                                    except:
                                        pass
                        except:
                            pass
            except:
                pass
    else:
        log("[WARNING] No root node!")

    log("")
    log("=" * 70)
    log("CHECKING ENEMY ACTOR FOR AI CONTROLLER")
    log("=" * 70)

    # Find enemy actors in the world
    actors = unreal.EditorLevelLibrary.get_all_level_actors()
    for actor in actors:
        actor_name = actor.get_name()
        if "Enemy" in actor_name and "Demo" in actor_name:
            log(f"Found enemy: {actor_name}")
            log(f"  Class: {actor.get_class().get_name()}")

            # Check AI controller class
            try:
                aic_class = actor.get_editor_property("ai_controller_class")
                if aic_class:
                    log(f"  AIControllerClass: {aic_class.get_name()}")
                else:
                    log(f"  AIControllerClass: None")
            except Exception as e:
                log(f"  AIControllerClass error: {e}")

            # Check auto possess
            try:
                auto_possess = actor.get_editor_property("auto_possess_ai")
                log(f"  AutoPossessAI: {auto_possess}")
            except:
                pass

            break

if __name__ == "__main__":
    main()
