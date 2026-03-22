#!/usr/bin/env python3
"""
Check if AB_SequenceActor class exists and can be loaded.
"""

import unreal

def log(msg):
    print(f"[CheckClass] {msg}")
    unreal.log_warning(f"[CheckClass] {msg}")

def main():
    log("=" * 70)
    log("CHECKING FOR C++ CLASSES")
    log("=" * 70)

    # Test various class paths
    test_paths = [
        "/Script/SLFConversion.AB_SequenceActor",
        "/Script/SLFConversion.B_SequenceActor",
        "/Script/SLFConversion.SLFSequenceActor",
        "/Script/SLFConversion.ASLFSequenceActor",
        "/Script/SLFConversion.SLFGameInstance",
        "/Script/SLFConversion.USLFGameInstance",
    ]

    for path in test_paths:
        log(f"Testing: {path}")
        try:
            cls = unreal.load_class(None, path)
            if cls:
                log(f"  FOUND: {cls.get_name()}")
            else:
                log(f"  NOT FOUND (returned None)")
        except Exception as e:
            log(f"  ERROR: {e}")

    # Also try finding any classes with "Sequence" in name
    log("")
    log("Looking for classes with 'Sequence' in name...")
    try:
        # Try to list classes from registry
        registry = unreal.AssetRegistryHelpers.get_asset_registry()
        if registry:
            log("  Registry available - but can't easily enumerate classes")
    except:
        pass

    # Check if the Blueprint parent is wrong
    log("")
    log("Checking B_SequenceActor Blueprint current parent...")
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor"
    try:
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if bp:
            log(f"  Loaded: {bp.get_name()}")
            try:
                gen_class = bp.generated_class()
                if gen_class:
                    parent = gen_class.get_super_class()
                    if parent:
                        log(f"  Current parent: {parent.get_name()} ({parent.get_path_name()})")
                    else:
                        log("  Current parent: None")
            except Exception as e:
                log(f"  Error getting parent: {e}")
        else:
            log(f"  Could not load Blueprint")
    except Exception as e:
        log(f"  Error loading Blueprint: {e}")

    log("=" * 70)
    log("DONE")
    log("=" * 70)

if __name__ == "__main__":
    main()
