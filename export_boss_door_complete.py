"""
Export COMPLETE B_BossDoor configuration from bp_only project.
Exports ALL CDO defaults, components, settings, hierarchy, and interfaces.
"""
import unreal
import json
import os

def export_boss_door_complete():
    output = {
        "ExportDate": "2026-01-14",
        "Project": "bp_only",
        "Purpose": "Complete B_BossDoor analysis for migration validation"
    }

    # Load the B_BossDoor Blueprint
    boss_door_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor"
    boss_door_bp = unreal.load_asset(boss_door_path)

    if not boss_door_bp:
        print(f"ERROR: Could not load {boss_door_path}")
        return

    print(f"Loaded: {boss_door_path}")

    # Get the generated class
    gen_class = boss_door_bp.generated_class()
    if not gen_class:
        print("ERROR: No generated class")
        return

    # Get CDO
    cdo = unreal.get_default_object(gen_class)
    if not cdo:
        print("ERROR: No CDO")
        return

    output["Blueprint"] = {
        "Path": boss_door_path,
        "ClassName": gen_class.get_name(),
        "ParentClass": str(gen_class.get_super_class().get_name()) if gen_class.get_super_class() else "None"
    }

    # Walk the full class hierarchy
    hierarchy = []
    current_class = gen_class
    while current_class:
        hierarchy.append({
            "ClassName": current_class.get_name(),
            "IsNative": not current_class.get_class().get_name().endswith("BlueprintGeneratedClass")
        })
        current_class = current_class.get_super_class()
    output["ClassHierarchy"] = hierarchy

    # Export ALL CDO properties
    print("\n=== CDO Properties ===")
    cdo_properties = {}

    # Use export_text to get all properties
    export_text = unreal.EditorAssetLibrary.get_metadata_tag(boss_door_path, "")

    # Get properties via iteration
    for prop in gen_class.properties():
        prop_name = prop.get_name()
        try:
            value = getattr(cdo, prop_name.lower().replace(" ", "_").replace("?", ""), None)
            if value is not None:
                cdo_properties[prop_name] = str(value)
        except:
            pass

    # Manually check critical properties
    critical_props = [
        "bCanBeTraced", "CanBeTraced?", "bIsActivated", "IsActivated?",
        "bIsFogWall", "IsFogWall?", "bSealed", "bIsLocked", "bAutoClose",
        "InteractableDisplayName", "InteractionText", "BossClass", "BossArenaTag"
    ]

    for prop_name in critical_props:
        try:
            # Try different naming conventions
            for attr_name in [prop_name, prop_name.lower(), prop_name.replace("?", "").lower(),
                            prop_name.replace("b", "", 1).lower() if prop_name.startswith("b") else prop_name]:
                if hasattr(cdo, attr_name):
                    val = getattr(cdo, attr_name)
                    cdo_properties[prop_name] = str(val)
                    print(f"  {prop_name}: {val}")
                    break
        except Exception as e:
            pass

    output["CDOProperties"] = cdo_properties

    # Export ALL components from CDO
    print("\n=== Components ===")
    components_data = []

    # Get all components
    all_components = cdo.get_components_by_class(unreal.ActorComponent)
    print(f"Found {len(all_components)} components")

    for comp in all_components:
        comp_info = {
            "Name": comp.get_name(),
            "Class": comp.get_class().get_name(),
        }

        # Scene component properties
        if isinstance(comp, unreal.SceneComponent):
            comp_info["RelativeLocation"] = {
                "X": comp.relative_location.x,
                "Y": comp.relative_location.y,
                "Z": comp.relative_location.z
            }
            comp_info["RelativeRotation"] = {
                "Pitch": comp.relative_rotation.pitch,
                "Yaw": comp.relative_rotation.yaw,
                "Roll": comp.relative_rotation.roll
            }
            comp_info["RelativeScale3D"] = {
                "X": comp.relative_scale3d.x,
                "Y": comp.relative_scale3d.y,
                "Z": comp.relative_scale3d.z
            }

            # Get attachment info
            parent = comp.get_attach_parent()
            comp_info["AttachedTo"] = parent.get_name() if parent else "None"

        # Static mesh component
        if isinstance(comp, unreal.StaticMeshComponent):
            mesh = comp.static_mesh
            comp_info["StaticMesh"] = str(mesh.get_path_name()) if mesh else "None"
            comp_info["Visibility"] = comp.is_visible()
            comp_info["CollisionEnabled"] = str(comp.get_collision_enabled())
            comp_info["CollisionObjectType"] = str(comp.get_collision_object_type())

        # Niagara component
        if comp.get_class().get_name() == "NiagaraComponent":
            try:
                asset = comp.get_editor_property("asset")
                comp_info["NiagaraAsset"] = str(asset.get_path_name()) if asset else "None"
            except:
                comp_info["NiagaraAsset"] = "Unable to read"
            comp_info["AutoActivate"] = comp.get_editor_property("auto_activate") if hasattr(comp, "auto_activate") else "Unknown"

        print(f"  {comp_info['Name']} ({comp_info['Class']})")
        if "RelativeScale3D" in comp_info:
            scale = comp_info["RelativeScale3D"]
            print(f"    Scale: ({scale['X']}, {scale['Y']}, {scale['Z']})")
        if "StaticMesh" in comp_info:
            print(f"    Mesh: {comp_info['StaticMesh']}")
        if "CollisionEnabled" in comp_info:
            print(f"    Collision: {comp_info['CollisionEnabled']}")

        components_data.append(comp_info)

    output["Components"] = components_data

    # Check SimpleConstructionScript (SCS) for Blueprint-added components
    print("\n=== SimpleConstructionScript (Blueprint Components) ===")
    scs_data = []

    try:
        # Access SCS through the Blueprint
        scs = boss_door_bp.get_editor_property("simple_construction_script")
        if scs:
            all_nodes = scs.get_all_nodes()
            print(f"Found {len(all_nodes)} SCS nodes")

            for node in all_nodes:
                node_info = {
                    "NodeName": node.get_name(),
                    "VariableName": str(node.get_editor_property("internal_variable_name")),
                }

                # Get component template
                comp_template = node.component_template
                if comp_template:
                    node_info["ComponentClass"] = comp_template.get_class().get_name()
                    node_info["ComponentName"] = comp_template.get_name()

                    if isinstance(comp_template, unreal.SceneComponent):
                        node_info["RelativeLocation"] = {
                            "X": comp_template.relative_location.x,
                            "Y": comp_template.relative_location.y,
                            "Z": comp_template.relative_location.z
                        }
                        node_info["RelativeRotation"] = {
                            "Pitch": comp_template.relative_rotation.pitch,
                            "Yaw": comp_template.relative_rotation.yaw,
                            "Roll": comp_template.relative_rotation.roll
                        }
                        node_info["RelativeScale3D"] = {
                            "X": comp_template.relative_scale3d.x,
                            "Y": comp_template.relative_scale3d.y,
                            "Z": comp_template.relative_scale3d.z
                        }

                    if isinstance(comp_template, unreal.StaticMeshComponent):
                        mesh = comp_template.static_mesh
                        node_info["StaticMesh"] = str(mesh.get_path_name()) if mesh else "None"
                        node_info["CollisionEnabled"] = str(comp_template.get_collision_enabled())

                    print(f"  SCS Node: {node_info['VariableName']} -> {node_info['ComponentClass']}")
                    if "RelativeScale3D" in node_info:
                        scale = node_info["RelativeScale3D"]
                        print(f"    Scale: ({scale['X']}, {scale['Y']}, {scale['Z']})")

                scs_data.append(node_info)
    except Exception as e:
        print(f"SCS access error: {e}")

    output["SimpleConstructionScript"] = scs_data

    # Check interfaces
    print("\n=== Implemented Interfaces ===")
    interfaces = []
    try:
        impl_interfaces = boss_door_bp.get_editor_property("implemented_interfaces")
        for iface in impl_interfaces:
            iface_class = iface.get_editor_property("interface")
            if iface_class:
                interfaces.append(iface_class.get_name())
                print(f"  {iface_class.get_name()}")
    except Exception as e:
        print(f"Interface access error: {e}")

    output["ImplementedInterfaces"] = interfaces

    # Check parent classes for their components too
    print("\n=== Parent Class Analysis ===")
    parent_analysis = []

    parent_paths = [
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable"
    ]

    for parent_path in parent_paths:
        parent_bp = unreal.load_asset(parent_path)
        if parent_bp:
            parent_info = {"Path": parent_path}
            parent_gen = parent_bp.generated_class()
            if parent_gen:
                parent_info["ClassName"] = parent_gen.get_name()
                parent_info["SuperClass"] = str(parent_gen.get_super_class().get_name()) if parent_gen.get_super_class() else "None"

                # Get SCS for parent
                try:
                    parent_scs = parent_bp.get_editor_property("simple_construction_script")
                    if parent_scs:
                        parent_nodes = parent_scs.get_all_nodes()
                        parent_info["SCSComponents"] = []
                        for node in parent_nodes:
                            comp_template = node.component_template
                            if comp_template:
                                comp_data = {
                                    "Name": str(node.get_editor_property("internal_variable_name")),
                                    "Class": comp_template.get_class().get_name()
                                }
                                if isinstance(comp_template, unreal.SceneComponent):
                                    comp_data["Scale"] = {
                                        "X": comp_template.relative_scale3d.x,
                                        "Y": comp_template.relative_scale3d.y,
                                        "Z": comp_template.relative_scale3d.z
                                    }
                                    comp_data["Location"] = {
                                        "X": comp_template.relative_location.x,
                                        "Y": comp_template.relative_location.y,
                                        "Z": comp_template.relative_location.z
                                    }
                                if isinstance(comp_template, unreal.StaticMeshComponent):
                                    mesh = comp_template.static_mesh
                                    comp_data["Mesh"] = str(mesh.get_path_name()) if mesh else "None"
                                parent_info["SCSComponents"].append(comp_data)
                                print(f"  {parent_path} -> {comp_data['Name']} ({comp_data['Class']})")
                except Exception as e:
                    print(f"  Parent SCS error: {e}")

            parent_analysis.append(parent_info)

    output["ParentClassAnalysis"] = parent_analysis

    # Save output
    output_path = "C:/scripts/SLFConversion/migration_cache/boss_door_complete_export.json"
    with open(output_path, 'w') as f:
        json.dump(output, f, indent=2)

    print(f"\n=== Saved complete export to {output_path} ===")

    # Also print critical summary
    print("\n" + "="*60)
    print("CRITICAL COMPONENT SUMMARY")
    print("="*60)

    for comp in components_data:
        if "Interactable" in comp["Name"] and "SM" in comp["Name"]:
            print(f"\nINTERACTABLE SM FOUND:")
            print(f"  Scale: {comp.get('RelativeScale3D', 'NOT FOUND')}")
            print(f"  Location: {comp.get('RelativeLocation', 'NOT FOUND')}")
            print(f"  Rotation: {comp.get('RelativeRotation', 'NOT FOUND')}")
            print(f"  Mesh: {comp.get('StaticMesh', 'NOT FOUND')}")
            print(f"  Collision: {comp.get('CollisionEnabled', 'NOT FOUND')}")

if __name__ == "__main__":
    export_boss_door_complete()
