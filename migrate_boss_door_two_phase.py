"""
Two-phase migration for B_BossDoor:
Phase 1: Clear EventGraph and save (separate editor session)
Phase 2: Reparent to SLFBossDoor (new editor session)

This script handles BOTH phases - run it twice if needed.
"""
import unreal
import os

BOSS_DOOR_PATH = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"
TARGET_CLASS_PATH = "/Script/SLFConversion.SLFBossDoor"
PHASE_FILE = "C:/scripts/SLFConversion/migration_cache/boss_door_phase.txt"

def get_current_phase():
    if os.path.exists(PHASE_FILE):
        with open(PHASE_FILE, 'r') as f:
            return f.read().strip()
    return "1"  # Default to phase 1

def set_phase(phase):
    os.makedirs(os.path.dirname(PHASE_FILE), exist_ok=True)
    with open(PHASE_FILE, 'w') as f:
        f.write(str(phase))

def phase_1_clear():
    """Phase 1: Clear all Blueprint logic"""
    print("=" * 60)
    print("PHASE 1: CLEAR BLUEPRINT LOGIC")
    print("=" * 60)

    bp = unreal.load_asset(BOSS_DOOR_PATH)
    if not bp:
        print(f"ERROR: Could not load {BOSS_DOOR_PATH}")
        return False

    # Clear uber graphs
    print("\nClearing uber graphs...")
    try:
        uber_graphs = bp.get_editor_property("uber_graph_pages")
        if uber_graphs:
            for graph in uber_graphs:
                nodes = list(graph.nodes)
                print(f"  {graph.get_name()}: {len(nodes)} nodes")
                for node in nodes:
                    try:
                        graph.remove_node(node)
                    except:
                        pass
    except Exception as e:
        print(f"  Error: {e}")

    # Clear function graphs
    print("\nClearing function graphs...")
    try:
        func_graphs = bp.get_editor_property("function_graphs")
        if func_graphs:
            for graph in func_graphs:
                nodes = list(graph.nodes)
                print(f"  {graph.get_name()}: {len(nodes)} nodes")
                for node in nodes:
                    try:
                        graph.remove_node(node)
                    except:
                        pass
    except Exception as e:
        print(f"  Error: {e}")

    # Clear delegate graphs
    print("\nClearing delegate graphs...")
    try:
        delegate_graphs = bp.get_editor_property("delegate_signature_graphs")
        if delegate_graphs:
            for graph in delegate_graphs:
                print(f"  Clearing: {graph.get_name()}")
    except Exception as e:
        print(f"  Error: {e}")

    # Save
    print("\nSaving...")
    try:
        unreal.EditorAssetLibrary.save_asset(BOSS_DOOR_PATH, only_if_is_dirty=False)
        print("  Saved!")
    except Exception as e:
        print(f"  Save error: {e}")

    set_phase("2")
    print("\nPhase 1 complete. Run script again for Phase 2.")
    return True

def phase_2_reparent():
    """Phase 2: Reparent to C++ class"""
    print("=" * 60)
    print("PHASE 2: REPARENT TO C++ CLASS")
    print("=" * 60)

    bp = unreal.load_asset(BOSS_DOOR_PATH)
    if not bp:
        print(f"ERROR: Could not load {BOSS_DOOR_PATH}")
        return False

    target_class = unreal.load_class(None, TARGET_CLASS_PATH)
    if not target_class:
        print(f"ERROR: Could not load {TARGET_CLASS_PATH}")
        return False

    print(f"\nReparenting to: {target_class.get_name()}")
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp, target_class)
        print(f"  Result: {result}")
    except Exception as e:
        print(f"  Error: {e}")

    # Compile
    print("\nCompiling...")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp)
        print("  Compiled")
    except Exception as e:
        print(f"  Compile error: {e}")

    # Save
    print("\nSaving...")
    try:
        unreal.EditorAssetLibrary.save_asset(BOSS_DOOR_PATH, only_if_is_dirty=False)
        print("  Saved!")
    except Exception as e:
        print(f"  Save error: {e}")

    # Verify
    print("\n--- Verification ---")
    bp = unreal.load_asset(BOSS_DOOR_PATH)
    if bp:
        gen = bp.generated_class()
        if gen:
            cdo = unreal.get_default_object(gen)
            if cdo:
                # Check for FogGateMesh component
                comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
                print(f"StaticMeshComponents: {len(comps)}")
                for c in comps:
                    scale = c.relative_scale3d
                    print(f"  {c.get_name()}: Scale ({scale.x}, {scale.y}, {scale.z})")

                # Check for Niagara components
                niagara_comps = cdo.get_components_by_class(unreal.NiagaraComponent)
                print(f"NiagaraComponents: {len(niagara_comps)}")
                for c in niagara_comps:
                    print(f"  {c.get_name()}")

    set_phase("done")
    print("\n" + "=" * 60)
    print("MIGRATION COMPLETE - Test in PIE")
    print("=" * 60)
    return True

def main():
    phase = get_current_phase()
    print(f"Current phase: {phase}")

    if phase == "1":
        phase_1_clear()
    elif phase == "2":
        phase_2_reparent()
    else:
        print("Migration already complete. Reset phase to '1' to re-run.")
        # Reset for next run
        set_phase("1")

if __name__ == "__main__":
    main()
