#!/usr/bin/env python3
"""
Diagnose why cinematic isn't playing.
Check B_SequenceActor configuration.
"""

import unreal

def log(msg):
    print(f"[DiagnoseCinematic] {msg}")
    unreal.log_warning(f"[DiagnoseCinematic] {msg}")

def main():
    log("=" * 70)
    log("DIAGNOSING CINEMATIC PLAYBACK")
    log("=" * 70)

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor"

    log(f"Loading B_SequenceActor Blueprint...")
    bp = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp:
        log(f"ERROR: Could not load Blueprint")
        return

    log(f"Blueprint loaded: {bp.get_name()}")

    # Get generated class
    gen_class = bp.generated_class()
    if gen_class:
        log(f"Generated class: {gen_class.get_name()}")

        # Get parent class
        try:
            parent_class = gen_class.static_class()
            log(f"Class: {parent_class.get_name()}")
        except:
            pass

    # Check CDO properties
    log("")
    log("Checking CDO properties...")
    try:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            log(f"CDO: {cdo.get_name()}")

            # Check SequenceToPlay
            try:
                seq = cdo.get_editor_property('sequence_to_play')
                log(f"  SequenceToPlay: {seq}")
            except Exception as e:
                log(f"  SequenceToPlay error: {e}")

            # Check Settings
            try:
                settings = cdo.get_editor_property('settings')
                log(f"  Settings: {settings}")
            except Exception as e:
                log(f"  Settings error: {e}")

            # Check CanBeSkipped
            try:
                skip = cdo.get_editor_property('can_be_skipped')
                log(f"  CanBeSkipped: {skip}")
            except Exception as e:
                log(f"  CanBeSkipped error: {e}")
    except Exception as e:
        log(f"CDO error: {e}")

    # Check SCS (SimpleConstructionScript) components
    log("")
    log("Checking SCS components...")
    try:
        scs = bp.get_editor_property('simple_construction_script')
        if scs:
            nodes = scs.get_all_nodes()
            log(f"  SCS nodes: {len(nodes)}")
            for node in nodes:
                comp_template = node.get_editor_property('component_template')
                if comp_template:
                    log(f"    - {comp_template.get_name()} ({comp_template.get_class().get_name()})")
    except Exception as e:
        log(f"SCS error: {e}")

    # Check if Blueprint has any event graphs
    log("")
    log("Checking EventGraph...")
    try:
        graphs = bp.get_editor_property('ubergraph_pages')
        log(f"  UberGraph pages: {len(graphs) if graphs else 0}")
    except Exception as e:
        log(f"EventGraph error: {e}")

    # Check C++ parent class
    log("")
    log("Checking C++ parent class...")
    cpp_path = "/Script/SLFConversion.B_SequenceActor"
    try:
        cpp_class = unreal.load_class(None, cpp_path)
        if cpp_class:
            log(f"C++ class: {cpp_class.get_name()}")
            log(f"  Path: {cpp_path}")
        else:
            log(f"C++ class not found!")
    except Exception as e:
        log(f"C++ class error: {e}")

    log("")
    log("=" * 70)
    log("DIAGNOSIS COMPLETE")
    log("=" * 70)

if __name__ == "__main__":
    main()
