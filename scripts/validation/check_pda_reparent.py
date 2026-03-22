"""Check PDA_DayNight parent class"""
import unreal

pda_path = "/Game/SoulslikeFramework/Data/PDA_DayNight"
pda = unreal.EditorAssetLibrary.load_asset(pda_path)
if pda:
    unreal.log_warning(f"=== PDA_DayNight ===")
    unreal.log_warning(f"  Asset type: {type(pda).__name__}")
    unreal.log_warning(f"  Class: {pda.get_class().get_name()}")

    # Check if it has generated_class
    if hasattr(pda, 'generated_class'):
        gen_class = pda.generated_class()
        if gen_class:
            unreal.log_warning(f"  Generated class: {gen_class.get_name()}")
            # Check parent via class default object
            parent_class = gen_class.static_class()
            if parent_class:
                unreal.log_warning(f"  Static class: {parent_class}")

# Also check the C++ class
cpp_class = unreal.load_class(None, "/Script/SLFConversion.PDA_DayNight")
if cpp_class:
    unreal.log_warning(f"\n=== C++ UPDA_DayNight ===")
    unreal.log_warning(f"  Class name: {cpp_class.get_name()}")

# Check DA_ExampleDayNightInfo
da_path = "/Game/SoulslikeFramework/Data/DayNight/DA_ExampleDayNightInfo"
da = unreal.EditorAssetLibrary.load_asset(da_path)
if da:
    unreal.log_warning(f"\n=== DA_ExampleDayNightInfo ===")
    unreal.log_warning(f"  Asset type: {type(da).__name__}")
    unreal.log_warning(f"  Class: {da.get_class().get_name()}")

    # Check if we can cast to UPDA_DayNight
    try:
        # Try getting entries - this will work if it's UPDA_DayNight
        entries = da.get_editor_property("entries")
        unreal.log_warning(f"  Entries accessible: {entries is not None}, count: {len(entries) if entries else 0}")
    except Exception as e:
        unreal.log_warning(f"  Entries access error: {e}")
