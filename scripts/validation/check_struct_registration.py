"""
Check how UE5 actually registers struct names.
"""
import unreal

print("=" * 60)
print("STRUCT REGISTRATION CHECK")
print("=" * 60)

# Try to find the structs by different names
struct_names_to_check = [
    # Without F prefix
    "/Script/SLFConversion.SLFSkeletalMeshData",
    "/Script/SLFConversion.StatInfo",
    "/Script/SLFConversion.SLFCardinalData",
    "/Script/SLFConversion.SLFItemInfo",

    # With F prefix
    "/Script/SLFConversion.FSLFSkeletalMeshData",
    "/Script/SLFConversion.FStatInfo",
    "/Script/SLFConversion.FSLFCardinalData",
    "/Script/SLFConversion.FSLFItemInfo",
]

for name in struct_names_to_check:
    try:
        # Try to load as a struct
        struct = unreal.find_object(None, name)
        if struct:
            print(f"[FOUND] {name}")
            print(f"        -> get_name(): {struct.get_name()}")
            print(f"        -> get_path_name(): {struct.get_path_name()}")
        else:
            print(f"[NOT FOUND] {name}")
    except Exception as e:
        print(f"[ERROR] {name}: {e}")

print("")
print("=" * 60)
print("CHECKING PROPERTY STRUCT TYPES")
print("=" * 60)

# Load a class that has these struct properties
try:
    # Load the PDA_DefaultMeshData class
    cls = unreal.load_class(None, "/Script/SLFConversion.PDA_DefaultMeshData")
    if cls:
        print(f"Loaded class: {cls.get_name()}")
        # Get CDO
        cdo = unreal.get_default_object(cls)
        if cdo:
            print(f"CDO: {cdo}")
            # Try to access the MeshData property
            try:
                mesh_data = cdo.get_editor_property('mesh_data')
                print(f"MeshData: {mesh_data}")
                print(f"MeshData type: {type(mesh_data)}")
            except Exception as e:
                print(f"MeshData error: {e}")
except Exception as e:
    print(f"Error loading class: {e}")

print("")
print("=" * 60)
print("CHECKING USTRUCT OBJECTS DIRECTLY")
print("=" * 60)

# Use StaticFindObject to find the struct
try:
    # This is how UE5 looks up structs internally
    from unreal import Name, Object

    # Try both naming conventions
    test_paths = [
        "/Script/SLFConversion.SLFSkeletalMeshData",
        "/Script/SLFConversion.FSLFSkeletalMeshData",
    ]

    for path in test_paths:
        obj = unreal.find_object(None, path)
        print(f"{path}: {obj}")

except Exception as e:
    print(f"Error: {e}")

print("")
print("Test complete!")
