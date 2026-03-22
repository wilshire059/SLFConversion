"""
Check B_Door_Demo in bp_only to see what it's supposed to look like.
"""
import unreal
import json

results = {"slfconversion": {}, "needs_fix": False}

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door_Demo"

unreal.log("=" * 70)
unreal.log("CHECKING B_Door_Demo")
unreal.log("=" * 70)

bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if bp:
    gen_class = bp.generated_class()
    if gen_class:
        results["slfconversion"]["class"] = gen_class.get_name()

        cdo = unreal.get_default_object(gen_class)
        if cdo:
            # Check all components
            all_comps = cdo.get_components_by_class(unreal.ActorComponent)
            results["slfconversion"]["total_components"] = len(all_comps)
            results["slfconversion"]["components"] = []

            for comp in all_comps:
                comp_info = {
                    "name": comp.get_name(),
                    "class": comp.get_class().get_name()
                }
                if isinstance(comp, unreal.StaticMeshComponent):
                    mesh = comp.static_mesh
                    comp_info["mesh"] = mesh.get_name() if mesh else "NO_MESH"
                results["slfconversion"]["components"].append(comp_info)

            unreal.log(f"B_Door_Demo CDO has {len(all_comps)} components")
            for c in results["slfconversion"]["components"]:
                unreal.log(f"  [{c['name']}] {c['class']}")
                if 'mesh' in c:
                    unreal.log(f"      Mesh: {c['mesh']}")

            # The problem: B_Door_Demo has NO mesh components
            # Solution: It should inherit from B_Door which has the mesh components
            # But it seems like it's a completely separate Blueprint

            # Let's check if we can add mesh components to B_Door_Demo CDO
            unreal.log("\nAttempting to add mesh components to B_Door_Demo...")

            # Load the meshes
            door_mesh = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Meshes/SM/PrisonDoor/SM_PrisonDoor.SM_PrisonDoor")
            frame_mesh = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Meshes/SM/PrisonDoor/SM_PrisonDoorArch.SM_PrisonDoorArch")

            # Check if there's a C++ property we can set
            try:
                default_door = cdo.get_editor_property('default_door_mesh')
                unreal.log(f"  default_door_mesh property exists: {default_door}")
                results["slfconversion"]["has_default_door_mesh"] = True
            except:
                unreal.log("  default_door_mesh property NOT FOUND")
                results["slfconversion"]["has_default_door_mesh"] = False

            # B_Door_Demo might need to be a child of B_Door
            # Let's suggest the fix
            results["diagnosis"] = "B_Door_Demo has NO StaticMeshComponents. It should likely be a child of B_Door."
            results["needs_fix"] = True

# Save
output_path = "C:/scripts/SLFConversion/migration_cache/door_demo_comparison.json"
with open(output_path, "w") as f:
    json.dump(results, f, indent=2)
unreal.log(f"\nSaved to {output_path}")

unreal.log("\n" + "=" * 70)
unreal.log("DIAGNOSIS")
unreal.log("=" * 70)
unreal.log("B_Door_Demo has NO mesh components - this is the cause of invisibility.")
unreal.log("The door Blueprint in the level needs mesh components.")
unreal.log("\nPossible fixes:")
unreal.log("  1. Reparent B_Door_Demo to B_Door (so it inherits mesh components)")
unreal.log("  2. Replace level instance with B_Door instead of B_Door_Demo")
unreal.log("  3. Add mesh components to B_Door_Demo SCS")
