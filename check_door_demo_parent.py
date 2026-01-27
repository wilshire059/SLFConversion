"""
Check B_Door_Demo parent class.
"""
import unreal
import json

results = {}

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door_Demo"

unreal.log("=" * 70)
unreal.log("B_Door_Demo PARENT CLASS CHECK")
unreal.log("=" * 70)

bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if bp:
    # Try to get parent class path using Blueprint property
    try:
        # BlueprintGeneratedClass has a function to get parent
        gen_class = bp.generated_class()
        if gen_class:
            results["generated_class"] = gen_class.get_name()
            unreal.log(f"Generated class: {gen_class.get_name()}")

            # Get CDO and its class path
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                unreal.log(f"CDO class: {cdo.get_class().get_name()}")
                results["cdo_class"] = cdo.get_class().get_name()

                # Try to get the outer class path
                outer = cdo.get_outer()
                if outer:
                    unreal.log(f"CDO outer: {outer.get_name()}")
                    results["cdo_outer"] = outer.get_name()

    except Exception as e:
        unreal.log(f"Error: {e}")

    # Also check using Blueprint's parent class property
    try:
        # Blueprints have a parent_class property
        parent = bp.get_editor_property('parent_class')
        if parent:
            unreal.log(f"Blueprint parent_class: {parent}")
            results["blueprint_parent_class"] = str(parent)
    except Exception as e:
        unreal.log(f"parent_class error: {e}")

# Also check what B_Door's class hierarchy looks like
unreal.log("\n" + "=" * 70)
unreal.log("B_Door CLASS CHECK (for comparison)")
unreal.log("=" * 70)

door_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"
door_bp = unreal.EditorAssetLibrary.load_asset(door_path)
if door_bp:
    gen_class = door_bp.generated_class()
    if gen_class:
        unreal.log(f"B_Door generated class: {gen_class.get_name()}")
        results["b_door_generated_class"] = gen_class.get_name()

        cdo = unreal.get_default_object(gen_class)
        if cdo:
            unreal.log(f"B_Door CDO class: {cdo.get_class().get_name()}")
            # Check components
            all_comps = cdo.get_components_by_class(unreal.ActorComponent)
            unreal.log(f"B_Door has {len(all_comps)} components")
            for comp in all_comps:
                unreal.log(f"  [{comp.get_name()}] {comp.get_class().get_name()}")
                if isinstance(comp, unreal.StaticMeshComponent):
                    mesh = comp.static_mesh
                    unreal.log(f"      Mesh: {mesh.get_name() if mesh else 'NO_MESH'}")

# Save
output_path = "C:/scripts/SLFConversion/migration_cache/door_demo_parent.json"
with open(output_path, "w") as f:
    json.dump(results, f, indent=2)
unreal.log(f"\nSaved to {output_path}")
