import unreal

output = []
def log(msg):
    print(msg)
    output.append(str(msg))

log("=" * 70)
log("APPLYING SEQUENCE ACTOR DATA")
log("=" * 70)

# Load the B_SequenceActor Blueprint
bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/B_SequenceActor"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

if bp:
    log(f"Loaded: {bp_path}")

    # Get the generated class to access CDO
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            log(f"CDO: {cdo.get_name()}")

            # Load the level sequence
            sequence_path = "/Game/SoulslikeFramework/Cinematics/LS_ShowcaseRoom"
            sequence = unreal.EditorAssetLibrary.load_asset(sequence_path)

            if sequence:
                log(f"Loaded sequence: {sequence_path}")

                # Try setting with Blueprint property name 'level_sequence'
                property_variants = [
                    "level_sequence",
                    "LevelSequence",
                ]

                success = False
                for prop_name in property_variants:
                    try:
                        cdo.set_editor_property(prop_name, sequence)
                        log(f"SUCCESS: Set {prop_name}")
                        success = True
                        break
                    except Exception as e:
                        log(f"  Failed {prop_name}: {e}")

                if success:
                    # Save the asset
                    unreal.EditorAssetLibrary.save_asset(bp_path)
                    log(f"Saved: {bp_path}")
            else:
                log(f"ERROR: Could not load sequence: {sequence_path}")
        else:
            log("ERROR: Could not get CDO")
    else:
        log("ERROR: Could not get generated class")
else:
    log(f"ERROR: Could not load: {bp_path}")

log("")
log("=" * 70)
log("Done!")
log("=" * 70)

# Write output to file
with open("C:/scripts/SLFConversion/migration_cache/sequence_actor_apply.txt", 'w') as f:
    f.write('\n'.join(output))
