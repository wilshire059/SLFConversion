"""
Extract AI weapon mesh data from backup (bp_only).
Run on bp_only project.
"""
import unreal
import json

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/ai_weapon_meshes.json"

AI_WEAPON_PATHS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_BossMace",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_DemoSword",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/AI/B_Item_AI_Weapon_DemoShield",
]

def extract_ai_weapon_meshes():
    output = {}

    for bp_path in AI_WEAPON_PATHS:
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"Could not load: {bp_path}")
            continue

        bp_name = bp.get_name()
        data = {"scs_meshes": [], "cdo_meshes": []}

        # Try to get the generated class
        try:
            gen_class = bp.generated_class()
            if not gen_class:
                unreal.log_warning(f"{bp_name}: No generated class")
                continue

            cdo = unreal.get_default_object(gen_class)

            # Check CDO for StaticMeshComponents
            try:
                mesh_comps = cdo.get_components_by_class(unreal.StaticMeshComponent)
                for comp in mesh_comps:
                    mesh = comp.get_editor_property("static_mesh")
                    data["cdo_meshes"].append({
                        "component_name": comp.get_name(),
                        "mesh_path": mesh.get_path_name() if mesh else None
                    })
                    unreal.log_warning(f"{bp_name} CDO {comp.get_name()}: {mesh.get_path_name() if mesh else '(None)'}")
            except Exception as e:
                unreal.log_warning(f"{bp_name} CDO mesh error: {e}")

            # Check for DefaultWeaponMesh property
            try:
                default_mesh = cdo.get_editor_property("default_weapon_mesh")
                if default_mesh:
                    data["default_weapon_mesh"] = default_mesh.get_path_name()
                    unreal.log_warning(f"{bp_name} DefaultWeaponMesh: {default_mesh.get_path_name()}")
                else:
                    data["default_weapon_mesh"] = None
            except Exception as e:
                data["default_weapon_mesh"] = None

        except Exception as e:
            unreal.log_warning(f"{bp_name} general error: {e}")

        # Try to check SCS (SimpleConstructionScript)
        try:
            scs = bp.simple_construction_script
            if scs:
                nodes = scs.get_all_nodes()
                for node in nodes:
                    if node.component_template:
                        comp = node.component_template
                        comp_class = comp.get_class().get_name()
                        if comp_class == "StaticMeshComponent":
                            try:
                                mesh = comp.get_editor_property("static_mesh")
                                data["scs_meshes"].append({
                                    "component_name": comp.get_name(),
                                    "mesh_path": mesh.get_path_name() if mesh else None
                                })
                                unreal.log_warning(f"{bp_name} SCS {comp.get_name()}: {mesh.get_path_name() if mesh else '(None)'}")
                            except Exception as e:
                                unreal.log_warning(f"{bp_name} SCS mesh error: {e}")
        except Exception as e:
            unreal.log_warning(f"{bp_name} SCS access error: {e}")

        output[bp_name] = data

    # Write to JSON
    with open(OUTPUT_FILE, 'w') as f:
        json.dump(output, f, indent=2)

    unreal.log_warning(f"\nExtracted {len(output)} AI weapons to {OUTPUT_FILE}")

if __name__ == "__main__":
    extract_ai_weapon_meshes()
