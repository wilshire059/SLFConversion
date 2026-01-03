# test_iterate.py
# Test simple iteration and load_asset
import unreal

def run():
    unreal.log_warning("=" * 60)
    unreal.log_warning("TEST ITERATION")
    unreal.log_warning("=" * 60)

    # Test dict items iteration
    test_map = {
        "AC_StatManager": "/Script/SLFConversion.StatManagerComponent",
        "AC_InputBuffer": "/Script/SLFConversion.InputBufferComponent",
    }

    for name, cpp in test_map.items():
        unreal.log_warning("Name type: " + str(type(name)))
        unreal.log_warning("Name: " + str(name))
        unreal.log_warning("Cpp type: " + str(type(cpp)))
        unreal.log_warning("Cpp: " + str(cpp))

    # Test load_asset
    path1 = "/Game/SoulslikeFramework/Blueprints/Components/AC_StatManager"
    unreal.log_warning("")
    unreal.log_warning("Path1 type: " + str(type(path1)))
    unreal.log_warning("Loading: " + path1)

    bp = unreal.EditorAssetLibrary.load_asset(path1)
    if bp:
        unreal.log_warning("Loaded: " + str(bp.get_name()))
    else:
        unreal.log_warning("Not found")

    # Test concatenation
    base = "/Game/SoulslikeFramework/Blueprints/Components"
    name = "AC_InputBuffer"
    full = base + "/" + name
    unreal.log_warning("")
    unreal.log_warning("Base type: " + str(type(base)))
    unreal.log_warning("Name type: " + str(type(name)))
    unreal.log_warning("Full type: " + str(type(full)))
    unreal.log_warning("Full: " + full)

    bp2 = unreal.EditorAssetLibrary.load_asset(full)
    if bp2:
        unreal.log_warning("Loaded: " + str(bp2.get_name()))
    else:
        unreal.log_warning("Not found")

    unreal.log_warning("")
    unreal.log_warning("Test complete!")

run()
