import unreal

bp = unreal.load_asset("/Game/SoulslikeFramework/Widgets/_Generic/W_GenericButton")
if bp:
    gen = bp.generated_class()
    if gen:
        cdo = unreal.get_default_object(gen)
        unreal.log_warning("CHECK: W_GenericButton CDO type: {}".format(type(cdo).__name__))
    else:
        unreal.log_warning("CHECK: W_GenericButton no generated class")
else:
    unreal.log_warning("CHECK: Could not load W_GenericButton")

unreal.log_warning("CHECK: Done!")
