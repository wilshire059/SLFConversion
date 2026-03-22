"""
Verify that BlendListByEnum nodes were migrated to C++ enum.
"""

import unreal

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("VERIFY BLEND LIST BY ENUM MIGRATION")
    unreal.log_warning("=" * 70)

    animbp = unreal.load_asset(ANIMBP_PATH)
    if not animbp:
        unreal.log_warning(f"[ERROR] Could not load AnimBP")
        return

    # Get all graphs
    graphs = list(animbp.get_editor_property('function_graphs'))

    unreal.log_warning(f"Checking {len(graphs)} graphs...")

    for graph in graphs:
        if not graph:
            continue

        graph_name = graph.get_name()
        nodes = list(graph.get_editor_property('nodes'))

        for node in nodes:
            if not node:
                continue

            class_name = node.get_class().get_name()
            if "BlendListByEnum" in class_name:
                # Get the bound enum
                try:
                    bound_enum = node.get_editor_property('bound_enum')
                    enum_path = bound_enum.get_path_name() if bound_enum else "None"
                    unreal.log_warning(f"")
                    unreal.log_warning(f"  Graph: {graph_name}")
                    unreal.log_warning(f"  Node: {node.get_name()}")
                    unreal.log_warning(f"  BoundEnum: {enum_path}")

                    if "/Script/SLFConversion" in enum_path:
                        unreal.log_warning(f"  [OK] Using C++ enum!")
                    elif "E_OverlayState" in enum_path:
                        unreal.log_warning(f"  [!!] Still using Blueprint enum!")
                    else:
                        unreal.log_warning(f"  [??] Unknown enum type")
                except Exception as e:
                    unreal.log_warning(f"  [ERROR] Could not get bound_enum: {e}")

    unreal.log_warning("")
    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
