"""Check parent classes of vendor widgets."""
import unreal

widgets = [
    "/Game/SoulslikeFramework/Widgets/Vendor/W_VendorSlot",
    "/Game/SoulslikeFramework/Widgets/Vendor/W_VendorAction",
    "/Game/SoulslikeFramework/Widgets/Vendor/W_NPC_Window_Vendor",
]

for path in widgets:
    bp = unreal.load_asset(path)
    if bp:
        gen_class = bp.generated_class()
        if gen_class:
            parent = gen_class.get_super_class()
            print(f"{bp.get_name()}: Parent = {parent.get_name() if parent else 'None'}")
        else:
            print(f"{bp.get_name()}: No generated_class")
    else:
        print(f"ERROR: Could not load {path}")
