import unreal
print("Testing WeaponDebugLibrary...")
try:
    cls = unreal.WeaponDebugLibrary
    print(f"Found class: {cls}")
    print(f"Methods: {dir(cls)}")
except Exception as e:
    print(f"ERROR: {e}")
