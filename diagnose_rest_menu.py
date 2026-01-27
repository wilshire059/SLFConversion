"""Diagnose Rest Menu time entries issue - simplified"""
import unreal
import json

def diagnose():
    unreal.log_warning("=" * 60)
    unreal.log_warning("REST MENU DIAGNOSTIC")
    unreal.log_warning("=" * 60)

    # Step 1: Check B_SkyManager Blueprint
    unreal.log_warning("\n=== 1. B_SkyManager Blueprint ===")
    sky_paths = [
        "/Game/SoulslikeFramework/Blueprints/Sky/B_SkyManager",
        "/Game/SoulslikeFramework/Blueprints/_Systems/B_SkyManager",
    ]

    sky_bp = None
    sky_path_found = None
    for path in sky_paths:
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if asset:
            unreal.log_warning(f"  Found at: {path}")
            sky_bp = asset
            sky_path_found = path
            break

    if not sky_bp:
        unreal.log_warning("  B_SkyManager NOT FOUND at any expected path")
        return

    # Get CDO to check TimeInfoAsset
    gen_class = sky_bp.generated_class()
    if gen_class:
        unreal.log_warning(f"  Generated class: {gen_class.get_name()}")
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            try:
                time_asset = cdo.get_editor_property("time_info_asset")
                if time_asset:
                    unreal.log_warning(f"  TimeInfoAsset: {time_asset.get_path_name()}")
                    unreal.log_warning(f"  TimeInfoAsset class: {time_asset.get_class().get_name()}")
                else:
                    unreal.log_warning("  TimeInfoAsset: None (NOT SET!)")
            except Exception as e:
                unreal.log_warning(f"  Error getting TimeInfoAsset: {e}")

    # Step 2: Check DA_ExampleDayNightInfo (the actual data instance)
    unreal.log_warning("\n=== 2. DA_ExampleDayNightInfo (Data Instance) ===")
    da_path = "/Game/SoulslikeFramework/Data/DayNight/DA_ExampleDayNightInfo"
    da = unreal.EditorAssetLibrary.load_asset(da_path)
    if da:
        unreal.log_warning(f"  Found: {da_path}")
        unreal.log_warning(f"  Class: {da.get_class().get_name()}")

        # Export text to understand structure
        try:
            text = da.export_text()
            unreal.log_warning(f"  Export text length: {len(text)}")

            # Save for analysis
            with open("C:/scripts/SLFConversion/migration_cache/da_daynight_export.txt", "w") as f:
                f.write(text)
            unreal.log_warning("  Saved to migration_cache/da_daynight_export.txt")

            # Count entries
            entry_count = text.count("(Tag=")
            unreal.log_warning(f"  Estimated entries: {entry_count}")
        except Exception as e:
            unreal.log_warning(f"  Export error: {e}")
    else:
        unreal.log_warning(f"  NOT FOUND: {da_path}")

    # Step 3: Check PDA_DayNight (the base Blueprint class)
    unreal.log_warning("\n=== 3. PDA_DayNight (Base Blueprint) ===")
    pda_path = "/Game/SoulslikeFramework/Data/PDA_DayNight"
    pda = unreal.EditorAssetLibrary.load_asset(pda_path)
    if pda:
        unreal.log_warning(f"  Found: {pda_path}")
        unreal.log_warning(f"  Class: {pda.get_class().get_name()}")
        gen_class = pda.generated_class()
        if gen_class:
            unreal.log_warning(f"  Generated class: {gen_class.get_name()}")
    else:
        unreal.log_warning(f"  NOT FOUND: {pda_path}")

    # Step 4: Check level for B_SkyManager actor
    unreal.log_warning("\n=== 4. Level Check for B_SkyManager Actor ===")
    try:
        editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
        world = editor_subsystem.get_editor_world()
        if world:
            unreal.log_warning(f"  Level: {world.get_name()}")

            all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)
            unreal.log_warning(f"  Total actors: {len(all_actors)}")

            sky_actors = []
            for actor in all_actors:
                name = actor.get_name()
                class_name = actor.get_class().get_name()
                if "sky" in name.lower() or "sky" in class_name.lower():
                    sky_actors.append((name, class_name, actor))

            if sky_actors:
                unreal.log_warning(f"  Found {len(sky_actors)} sky-related actors:")
                for name, class_name, actor in sky_actors:
                    unreal.log_warning(f"    - {name} ({class_name})")
                    # Check if it has TimeInfoAsset
                    try:
                        time_asset = actor.get_editor_property("time_info_asset")
                        if time_asset:
                            unreal.log_warning(f"      TimeInfoAsset: {time_asset.get_path_name()}")
                        else:
                            unreal.log_warning(f"      TimeInfoAsset: None")
                    except Exception as e:
                        unreal.log_warning(f"      No TimeInfoAsset property: {e}")
            else:
                unreal.log_warning("  No sky-related actors found in level!")
        else:
            unreal.log_warning("  No editor world available")
    except Exception as e:
        unreal.log_warning(f"  Level check error: {e}")

    unreal.log_warning("\n=== SUMMARY ===")
    unreal.log_warning("1. B_SkyManager Blueprint location: /Blueprints/Sky/ (not /Blueprints/_Systems/)")
    unreal.log_warning("2. Need to check if B_SkyManager actor exists in level with TimeInfoAsset set")
    unreal.log_warning("3. Need to ensure PDA_DayNight has Entries data populated")

diagnose()
