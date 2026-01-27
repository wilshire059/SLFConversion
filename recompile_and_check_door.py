"""
Force recompile B_Door and check components
"""
import unreal
import json

bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door"

unreal.log(f"Loading Blueprint: {bp_path}")
bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if not bp:
    unreal.log_error(f"ERROR: Could not load {bp_path}")
else:
    unreal.log(f"Loaded: {bp.get_name()}")

    # Force recompile
    unreal.log("Forcing Blueprint recompile...")
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    unreal.log("Compiled!")

    # Save
    unreal.log("Saving...")
    unreal.EditorAssetLibrary.save_asset(bp_path)

    # Check generated class
    gen_class = bp.generated_class()
    if gen_class:
        unreal.log(f"Generated class: {gen_class.get_name()}")

        # Get CDO
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            unreal.log(f"CDO: {cdo.get_name()}")

            # List all components
            all_comps = cdo.get_components_by_class(unreal.SceneComponent)
            unreal.log(f"=== Blueprint CDO SceneComponents ({len(all_comps)}) ===")
            for comp in all_comps:
                if isinstance(comp, unreal.StaticMeshComponent):
                    mesh = comp.static_mesh
                    mesh_name = mesh.get_name() if mesh else "NO_MESH"
                    unreal.log(f"  COMP: {comp.get_name()} StaticMesh mesh={mesh_name} visible={comp.is_visible()}")
                else:
                    unreal.log(f"  COMP: {comp.get_name()} {comp.get_class().get_name()} visible={comp.is_visible()}")

    # Also check C++ parent CDO
    unreal.log("=== Checking C++ Parent CDO ===")
    cpp_class = unreal.load_class(None, "/Script/SLFConversion.B_Door")
    if cpp_class:
        unreal.log(f"C++ Class: {cpp_class.get_name()}")
        cpp_cdo = unreal.get_default_object(cpp_class)
        if cpp_cdo:
            unreal.log(f"C++ CDO: {cpp_cdo.get_name()}")
            all_comps = cpp_cdo.get_components_by_class(unreal.SceneComponent)
            unreal.log(f"=== C++ CDO SceneComponents ({len(all_comps)}) ===")
            for comp in all_comps:
                if isinstance(comp, unreal.StaticMeshComponent):
                    mesh = comp.static_mesh
                    mesh_name = mesh.get_name() if mesh else "NO_MESH"
                    unreal.log(f"  CPP_COMP: {comp.get_name()} StaticMesh mesh={mesh_name} visible={comp.is_visible()}")
                else:
                    unreal.log(f"  CPP_COMP: {comp.get_name()} {comp.get_class().get_name()} visible={comp.is_visible()}")

# Write results to file for easier reading
results = {
    "blueprint_cdo": [],
    "cpp_cdo": []
}

bp = unreal.EditorAssetLibrary.load_asset(bp_path)
if bp:
    gen_class = bp.generated_class()
    if gen_class:
        cdo = unreal.get_default_object(gen_class)
        if cdo:
            for comp in cdo.get_components_by_class(unreal.SceneComponent):
                info = {"name": comp.get_name(), "class": comp.get_class().get_name(), "visible": comp.is_visible()}
                if isinstance(comp, unreal.StaticMeshComponent):
                    mesh = comp.static_mesh
                    info["mesh"] = mesh.get_name() if mesh else None
                results["blueprint_cdo"].append(info)

cpp_class = unreal.load_class(None, "/Script/SLFConversion.B_Door")
if cpp_class:
    cpp_cdo = unreal.get_default_object(cpp_class)
    if cpp_cdo:
        for comp in cpp_cdo.get_components_by_class(unreal.SceneComponent):
            info = {"name": comp.get_name(), "class": comp.get_class().get_name(), "visible": comp.is_visible()}
            if isinstance(comp, unreal.StaticMeshComponent):
                mesh = comp.static_mesh
                info["mesh"] = mesh.get_name() if mesh else None
            results["cpp_cdo"].append(info)

with open("C:/scripts/SLFConversion/migration_cache/door_cdo_check.json", "w") as f:
    json.dump(results, f, indent=2)
unreal.log("Results saved to door_cdo_check.json")
