import unreal

bp_path = '/Game/SoulslikeFramework/Blueprints/Global/PC_SoulslikeFramework'
bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if bp:
    unreal.log(f"=== Blueprint: {bp.get_name()} ===")

    gen_class = bp.generated_class()
    if gen_class:
        # Get parent via cast
        parent_class = unreal.SystemLibrary.get_class(unreal.get_default_object(gen_class))
        unreal.log(f"Generated class: {gen_class.get_name()}")

        cdo = unreal.get_default_object(gen_class)
        unreal.log(f"CDO class: {cdo.get_class().get_name()}")

        # Check for AC_ProgressManager property
        try:
            pm = cdo.get_editor_property('ac_progress_manager')
            unreal.log(f"ac_progress_manager property: {pm}")
            if pm:
                unreal.log(f"  - Type: {pm.get_class().get_name()}")
        except Exception as e:
            unreal.log_warning(f"ac_progress_manager not found: {e}")

    # List SCS components
    if hasattr(bp, 'simple_construction_script') and bp.simple_construction_script:
        scs = bp.simple_construction_script
        unreal.log("=== SCS Components ===")
        nodes = scs.get_all_nodes() if hasattr(scs, 'get_all_nodes') else []
        if len(nodes) == 0:
            unreal.log("  (no SCS nodes)")
        for node in nodes:
            if node and hasattr(node, 'component_template') and node.component_template:
                comp = node.component_template
                unreal.log(f"  {comp.get_name()} ({comp.get_class().get_name()})")
    else:
        unreal.log("=== No SCS ===")
else:
    unreal.log_error("Blueprint not found")
