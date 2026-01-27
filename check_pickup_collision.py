"""
Check B_PickupItem collision settings in the backup project
"""
import unreal

def check_pickup_collision():
    """Check the collision settings of B_PickupItem's Support Collision component"""

    # Load B_PickupItem Blueprint
    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/B_PickupItem"

    unreal.log(f"\n{'='*60}")
    unreal.log(f"Checking B_PickupItem collision settings")
    unreal.log(f"{'='*60}")

    bp_asset = unreal.EditorAssetLibrary.load_asset(bp_path)
    if not bp_asset:
        unreal.log(f"ERROR: Could not load {bp_path}")
        return

    unreal.log(f"Loaded: {bp_asset.get_name()}")

    # Get the Blueprint generated class
    gen_class = unreal.EditorAssetLibrary.load_blueprint_class(bp_path)
    if not gen_class:
        unreal.log(f"ERROR: Could not load generated class")
        return

    # Get the CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        unreal.log(f"ERROR: Could not get CDO")
        return

    unreal.log(f"CDO: {cdo.get_name()}")
    unreal.log(f"Class: {cdo.get_class().get_name()}")

    # Check if CanBeTraced property exists and its value
    try:
        can_be_traced = cdo.get_editor_property("can_be_traced")
        unreal.log(f"CanBeTraced: {can_be_traced}")
    except:
        unreal.log("CanBeTraced property not found directly")

    # Get all components
    unreal.log(f"\n--- Components ---")
    components = cdo.get_components_by_class(unreal.ActorComponent)
    for comp in components:
        unreal.log(f"  Component: {comp.get_name()} ({comp.get_class().get_name()})")

        # If it's a primitive component, check collision settings
        if isinstance(comp, unreal.PrimitiveComponent):
            try:
                collision_enabled = comp.get_collision_enabled()
                object_type = comp.get_collision_object_type()
                unreal.log(f"    CollisionEnabled: {collision_enabled}")
                unreal.log(f"    CollisionObjectType: {object_type}")
            except Exception as e:
                unreal.log(f"    Collision info error: {e}")

    # Also check the SimpleConstructionScript
    unreal.log(f"\n--- SCS Components ---")
    try:
        # Access SCS through the Blueprint
        scs = bp_asset.simple_construction_script
        if scs:
            all_nodes = scs.get_all_nodes()
            for node in all_nodes:
                comp_template = node.component_template
                if comp_template:
                    unreal.log(f"  SCS Node: {comp_template.get_name()} ({comp_template.get_class().get_name()})")
                    if isinstance(comp_template, unreal.PrimitiveComponent):
                        try:
                            collision_enabled = comp_template.get_collision_enabled()
                            object_type = comp_template.get_collision_object_type()
                            unreal.log(f"    CollisionEnabled: {collision_enabled}")
                            unreal.log(f"    CollisionObjectType: {object_type}")

                            # Check collision responses
                            unreal.log(f"    CollisionResponses:")
                            for channel in range(32):  # Check first 32 channels
                                try:
                                    response = comp_template.get_collision_response_to_channel(channel)
                                    if response != unreal.CollisionResponseType.IGNORE:
                                        unreal.log(f"      Channel {channel}: {response}")
                                except:
                                    pass
                        except Exception as e:
                            unreal.log(f"    Collision info error: {e}")
    except Exception as e:
        unreal.log(f"  SCS access error: {e}")

if __name__ == "__main__":
    check_pickup_collision()
