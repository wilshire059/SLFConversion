import unreal

# Load the AC_RadarManager Blueprint
bp_path = "/Game/SoulslikeFramework/Blueprints/Components/AC_RadarManager"
bp = unreal.load_asset(bp_path)

if bp:
    print(f"=== Loaded: {bp_path} ===")

    # Get the generated class
    gen_class = bp.generated_class()
    if gen_class:
        print(f"Generated class: {gen_class.get_name()}")

        # Get CDO
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            print(f"CDO: {cdo.get_name()}")

            # Try to get CardinalData
            try:
                cardinal_data = cdo.get_editor_property("CardinalData")
                print(f"\nCardinalData type: {type(cardinal_data)}")
                print(f"CardinalData entries: {len(cardinal_data) if cardinal_data else 0}")

                if cardinal_data:
                    for tag, data in cardinal_data.items():
                        print(f"  Tag: {tag}")
                        print(f"    UIDisplayText: {data.get_editor_property('UIDisplayText') if hasattr(data, 'get_editor_property') else data}")
                        print(f"    Value: {data.get_editor_property('Value') if hasattr(data, 'get_editor_property') else 'N/A'}")
            except Exception as e:
                print(f"Error getting CardinalData: {e}")

            # Try to get RadarClampLength
            try:
                clamp = cdo.get_editor_property("RadarClampLength")
                print(f"\nRadarClampLength: {clamp}")
            except Exception as e:
                print(f"Error getting RadarClampLength: {e}")

            # Try to get RefreshInterval
            try:
                interval = cdo.get_editor_property("RefreshInterval")
                print(f"RefreshInterval: {interval}")
            except Exception as e:
                print(f"Error getting RefreshInterval: {e}")

            # List all properties
            print("\n=== All CDO Properties ===")
            for prop in dir(cdo):
                if not prop.startswith('_'):
                    try:
                        val = getattr(cdo, prop)
                        if not callable(val):
                            print(f"  {prop}: {val}")
                    except:
                        pass
else:
    print(f"Failed to load: {bp_path}")
