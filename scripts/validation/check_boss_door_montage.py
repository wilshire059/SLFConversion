import unreal

# Check AM_SLF_OpenDoor_Boss montage length
unreal.log("=== Boss Door Montage Analysis ===")

montage_path = "/Game/SoulslikeFramework/Demo/_Animations/Interaction/Door/AM_SLF_OpenDoor_Boss"
montage = unreal.load_asset(montage_path)

if montage:
    unreal.log(f"Montage: {montage.get_name()}")
    unreal.log(f"Sequence Length: {montage.sequence_length}s")
else:
    unreal.log("Boss montage not found!")

# Also check the LH/RH versions
for suffix in ["_RH", "_LH"]:
    path = f"/Game/SoulslikeFramework/Demo/_Animations/Interaction/Door/AM_SLF_OpenDoor{suffix}"
    m = unreal.load_asset(path)
    if m:
        unreal.log(f"Montage: {m.get_name()} - Length: {m.sequence_length}s")

unreal.log("=== Summary ===")
unreal.log("The original Blueprint uses OnNotifyBegin callback from PlayMontage.")
unreal.log("When an anim notify fires in the montage, it triggers movement.")
unreal.log("My C++ uses timer delay instead, which is why timing may be off.")
