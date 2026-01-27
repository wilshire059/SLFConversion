"""Check PDA_DayNight entries in bp_only"""
import unreal

# Check PDA_DayNight
pda_path = "/Game/SoulslikeFramework/Data/PDA_DayNight"
pda = unreal.EditorAssetLibrary.load_asset(pda_path)
if pda:
    unreal.log_warning(f"=== PDA_DayNight found ===")
    unreal.log_warning(f"  Class: {pda.get_class().get_name()}")
    unreal.log_warning(f"  Path: {pda.get_path_name()}")

    # Get generated class for Blueprint
    gen_class = pda.generated_class()
    if gen_class:
        unreal.log_warning(f"  Generated class: {gen_class.get_name()}")

        # Get the CDO
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            unreal.log_warning(f"  CDO: {cdo.get_name()}")

            # Try to get Entries
            try:
                entries = cdo.get_editor_property("entries")
                if entries:
                    unreal.log_warning(f"  Entries count: {len(entries)}")
                    for i, entry in enumerate(entries):
                        try:
                            name = entry.get_editor_property("name")
                            from_time = entry.get_editor_property("from_time")
                            to_time = entry.get_editor_property("to_time")
                            unreal.log_warning(f"    [{i}] {name} (from: {from_time}, to: {to_time})")
                        except Exception as e:
                            unreal.log_warning(f"    [{i}] Error reading entry: {e}")
                else:
                    unreal.log_warning(f"  Entries: Empty or None")
            except Exception as e:
                unreal.log_warning(f"  Could not get entries: {e}")
    else:
        unreal.log_warning(f"  No generated class (pure Blueprint)")

        # Try export_text approach
        try:
            text = pda.export_text()
            # Search for Entries in text
            if "Entries" in text:
                unreal.log_warning(f"  Found 'Entries' in export text")
                # Find the lines with Entries
                for line in text.split('\n'):
                    if 'Entries' in line or 'Name=' in line:
                        unreal.log_warning(f"    {line.strip()[:100]}")
        except:
            pass
else:
    unreal.log_warning(f"PDA_DayNight not found")
