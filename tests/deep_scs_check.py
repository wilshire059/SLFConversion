import unreal

# Try different ways to access SCS
bp_path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
bp = unreal.EditorAssetLibrary.load_asset(bp_path)

unreal.log_warning("=== Inspecting B_Soulslike_Character ===")
unreal.log_warning("Blueprint type: " + str(type(bp)))
unreal.log_warning("Blueprint name: " + bp.get_name())

# List all attributes
attrs = dir(bp)
scs_related = [a for a in attrs if 'scs' in a.lower() or 'simple' in a.lower() or 'construct' in a.lower() or 'component' in a.lower()]
unreal.log_warning("SCS-related attributes: " + str(scs_related))

# Check generated class
gen_class = bp.generated_class()
if gen_class:
    unreal.log_warning("Generated class: " + gen_class.get_name())
    
    # Get CDO and list component properties
    cdo = unreal.get_default_object(gen_class)
    if cdo:
        unreal.log_warning("CDO name: " + cdo.get_name())
        
        # Try to get all components from CDO
        if hasattr(cdo, 'get_components_by_class'):
            try:
                all_comps = cdo.get_components_by_class(unreal.ActorComponent)
                unreal.log_warning("Components via get_components_by_class: " + str(len(all_comps)))
                for comp in all_comps[:20]:
                    unreal.log_warning("  - " + comp.get_name() + " (" + comp.get_class().get_name() + ")")
            except Exception as e:
                unreal.log_warning("Error getting components: " + str(e))

# Check if we can access via editor property
try:
    scs = bp.get_editor_property("simple_construction_script")
    if scs:
        unreal.log_warning("SCS via get_editor_property: EXISTS")
        nodes = scs.get_all_nodes()
        unreal.log_warning("SCS Nodes: " + str(len(nodes)))
    else:
        unreal.log_warning("SCS via get_editor_property: None")
except Exception as e:
    unreal.log_warning("SCS access error: " + str(e))
