"""Check for SkyManager in the level and verify PDA_DayNight data"""
import unreal

# Check for any SkyManager actors in the level
world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
if not world:
    unreal.log_warning("No editor world found")
else:
    unreal.log_warning(f"=== Checking level: {world.get_name()} ===")

    # Get all actors
    all_actors = unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)
    unreal.log_warning(f"Total actors in level: {len(all_actors)}")

    # Find any actor with "Sky" or "Manager" in the name
    sky_actors = []
    for actor in all_actors:
        name = actor.get_name()
        class_name = actor.get_class().get_name()
        if "sky" in name.lower() or "sky" in class_name.lower():
            sky_actors.append((name, class_name))

    unreal.log_warning(f"\n=== Actors with 'Sky' in name/class ({len(sky_actors)}): ===")
    for name, class_name in sky_actors:
        unreal.log_warning(f"  {name} ({class_name})")

# Check if B_SkyManager class exists
sky_manager_path = "/Game/SoulslikeFramework/Blueprints/_Systems/B_SkyManager"
sky_manager_bp = unreal.EditorAssetLibrary.load_asset(sky_manager_path)
if sky_manager_bp:
    unreal.log_warning(f"\n=== B_SkyManager Blueprint exists ===")
    gen_class = sky_manager_bp.generated_class()
    if gen_class:
        unreal.log_warning(f"  Generated class: {gen_class.get_name()}")
        unreal.log_warning(f"  Parent class: {gen_class.get_super_class().get_name()}")
else:
    unreal.log_warning(f"\n=== B_SkyManager Blueprint NOT found at {sky_manager_path} ===")

# Check PDA_DayNight
pda_path = "/Game/SoulslikeFramework/Data/PDA_DayNight"
pda = unreal.EditorAssetLibrary.load_asset(pda_path)
if pda:
    unreal.log_warning(f"\n=== PDA_DayNight found ===")
    unreal.log_warning(f"  Class: {pda.get_class().get_name()}")

    # Try to get Entries
    try:
        entries = pda.get_editor_property("entries")
        if entries:
            unreal.log_warning(f"  Entries count: {len(entries)}")
            for i, entry in enumerate(entries):
                try:
                    name = entry.get_editor_property("name")
                    unreal.log_warning(f"    [{i}] {name}")
                except:
                    unreal.log_warning(f"    [{i}] (could not read name)")
        else:
            unreal.log_warning(f"  Entries: Empty or None")
    except Exception as e:
        unreal.log_warning(f"  Could not get entries property: {e}")
else:
    unreal.log_warning(f"\n=== PDA_DayNight NOT found at {pda_path} ===")

# Try to find any PDA_DayNight assets
unreal.log_warning("\n=== Searching for DayNight assets ===")
all_assets = unreal.EditorAssetLibrary.list_assets("/Game/SoulslikeFramework/Data", recursive=True)
for asset_path in all_assets:
    if "daynight" in asset_path.lower() or "day_night" in asset_path.lower():
        unreal.log_warning(f"  Found: {asset_path}")
