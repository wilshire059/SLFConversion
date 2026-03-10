# extract_npc_data.py
# Extracts NPC data from B_Soulslike_NPC variants including:
# - AC_AI_InteractionManager component data (Name, DialogAsset, VendorAsset)
# - Mesh data (SkeletalMesh assigned to character)
# - AnimClass (Animation Blueprint)
#
# Run on bp_only project BEFORE migration to capture original values:
# UnrealEditor-Cmd.exe "C:/scripts/bp_only/bp_only.uproject" -run=pythonscript -script="C:/scripts/SLFConversion/extract_npc_data.py" -stdout -unattended -nosplash

import unreal
import json
import os

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/npc_data.json"
DEBUG_FILE = "C:/scripts/SLFConversion/npc_extract_debug.txt"

# NPC Blueprint paths to extract
NPC_ASSETS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide",
]

def debug_log(msg):
    """Write debug message to file and console"""
    with open(DEBUG_FILE, 'a') as f:
        f.write(msg + "\n")
    print(msg)

def get_property_value(obj, prop_name):
    """Try to get a property value from an object using various methods"""
    # Try direct get_editor_property first
    try:
        return obj.get_editor_property(prop_name)
    except:
        pass

    # Try attribute access
    try:
        return getattr(obj, prop_name)
    except:
        pass

    # Try with underscore/snake_case variation
    try:
        snake_name = ''.join(['_' + c.lower() if c.isupper() else c for c in prop_name]).lstrip('_')
        return obj.get_editor_property(snake_name)
    except:
        pass

    return None

def list_all_properties(obj, prefix=""):
    """List all accessible properties on an object"""
    props = {}
    for attr in dir(obj):
        if attr.startswith('_'):
            continue
        try:
            val = getattr(obj, attr)
            if not callable(val):
                props[attr] = str(val)[:100]
        except:
            pass
    return props

def extract_component_data(component, comp_name):
    """Extract data from a component"""
    data = {
        "class": str(type(component).__name__),
        "name": comp_name
    }

    # For AC_AI_InteractionManager, extract key properties
    if "InteractionManager" in comp_name or "InteractionManager" in str(type(component)):
        # Name
        name_val = get_property_value(component, "Name")
        if name_val is None:
            name_val = get_property_value(component, "name")
        if name_val is not None:
            if hasattr(name_val, '__str__'):
                data["npc_name"] = str(name_val)
            else:
                data["npc_name"] = name_val

        # DialogAsset
        dialog_val = get_property_value(component, "DialogAsset")
        if dialog_val is None:
            dialog_val = get_property_value(component, "dialog_asset")
        if dialog_val is not None:
            if hasattr(dialog_val, 'get_path_name'):
                data["dialog_asset"] = dialog_val.get_path_name()
            else:
                data["dialog_asset"] = str(dialog_val)

        # VendorAsset
        vendor_val = get_property_value(component, "VendorAsset")
        if vendor_val is None:
            vendor_val = get_property_value(component, "vendor_asset")
        if vendor_val is not None:
            if hasattr(vendor_val, 'get_path_name'):
                data["vendor_asset"] = vendor_val.get_path_name()
            else:
                data["vendor_asset"] = str(vendor_val)

        debug_log(f"    InteractionManager: Name={data.get('npc_name', 'N/A')}, Dialog={data.get('dialog_asset', 'N/A')}, Vendor={data.get('vendor_asset', 'N/A')}")

    return data

def extract_from_spawned_actor(bp_path, bp_name):
    """
    Spawn the Blueprint actor and extract component data at runtime.
    This is needed because component default values are only accessible on spawned instances.
    """
    data = {
        "path": bp_path,
        "name": bp_name,
        "components": {},
        "mesh": None,
        "anim_class": None
    }

    try:
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            debug_log(f"  Could not load Blueprint: {bp_path}")
            return data

        gen_class = bp.generated_class()
        if not gen_class:
            debug_log(f"  Could not get generated class")
            return data

        # Spawn actor in editor world
        world = unreal.EditorLevelLibrary.get_editor_world()
        if not world:
            debug_log(f"  Could not get editor world")
            return data

        spawn_loc = unreal.Vector(0, 0, -10000)  # Spawn below level
        spawn_rot = unreal.Rotator(0, 0, 0)
        actor = world.spawn_actor(gen_class, spawn_loc, spawn_rot)

        if not actor:
            debug_log(f"  Could not spawn actor")
            return data

        debug_log(f"  Spawned actor: {actor.get_name()}")

        # Get SkeletalMeshComponent (character mesh)
        try:
            mesh_comp = actor.mesh  # ACharacter has 'mesh' property
            if mesh_comp is None:
                mesh_comp = actor.get_mesh()
            if mesh_comp:
                skel_mesh = mesh_comp.get_skeletal_mesh_asset()
                if skel_mesh:
                    data["mesh"] = skel_mesh.get_path_name()
                    debug_log(f"    Mesh: {data['mesh']}")

                anim_class = mesh_comp.get_anim_class()
                if anim_class:
                    data["anim_class"] = anim_class.get_path_name()
                    debug_log(f"    AnimClass: {data['anim_class']}")
        except Exception as e:
            debug_log(f"    Error getting mesh: {e}")

        # Get all components
        components = actor.get_components_by_class(unreal.ActorComponent)
        debug_log(f"  Found {len(components)} components")

        for comp in components:
            comp_name = comp.get_name()
            comp_class = type(comp).__name__

            # Focus on AC_AI_InteractionManager
            if "InteractionManager" in comp_name or "AI_Interaction" in comp_name:
                debug_log(f"  Processing component: {comp_name} ({comp_class})")
                comp_data = extract_component_data(comp, comp_name)
                data["components"][comp_name] = comp_data

        # Destroy the spawned actor
        actor.destroy_actor()
        debug_log(f"  Destroyed spawned actor")

    except Exception as e:
        debug_log(f"  Error spawning actor: {e}")
        import traceback
        debug_log(traceback.format_exc())

    return data

def extract_from_scs(bp_path, bp_name):
    """
    Try to extract component defaults from Blueprint's SCS (SimpleConstructionScript).
    This accesses the component templates without spawning.
    """
    data = {
        "path": bp_path,
        "name": bp_name,
        "scs_components": {}
    }

    try:
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            return data

        # Try to access SimpleConstructionScript
        scs = None
        try:
            scs = bp.get_editor_property("simple_construction_script")
        except:
            pass

        if scs:
            try:
                all_nodes = scs.get_all_nodes()
                debug_log(f"  SCS has {len(all_nodes)} nodes")

                for node in all_nodes:
                    if node:
                        comp_template = node.get_editor_property("component_template")
                        if comp_template:
                            comp_name = comp_template.get_name()
                            comp_class = type(comp_template).__name__

                            if "InteractionManager" in comp_name:
                                debug_log(f"    SCS Component: {comp_name} ({comp_class})")
                                comp_data = extract_component_data(comp_template, comp_name)
                                data["scs_components"][comp_name] = comp_data
            except Exception as e:
                debug_log(f"  Error accessing SCS nodes: {e}")
        else:
            debug_log(f"  No SCS found")

    except Exception as e:
        debug_log(f"  Error in SCS extraction: {e}")

    return data

def extract_npc_data():
    """
    Extract NPC data from Blueprints.
    Combines SCS template extraction with spawned actor extraction.
    """
    # Clear debug file
    with open(DEBUG_FILE, 'w') as f:
        f.write("")

    # Ensure output directory exists
    os.makedirs(os.path.dirname(OUTPUT_FILE), exist_ok=True)

    debug_log("")
    debug_log("=" * 70)
    debug_log("EXTRACTING NPC DATA FROM BLUEPRINTS")
    debug_log("=" * 70)

    npc_data = {}

    for bp_path in NPC_ASSETS:
        bp_name = bp_path.split("/")[-1]
        debug_log(f"\n[{bp_name}]")

        # Try spawned actor extraction (most reliable for runtime values)
        spawned_data = extract_from_spawned_actor(bp_path, bp_name)

        # Try SCS extraction (for template values)
        scs_data = extract_from_scs(bp_path, bp_name)

        # Merge data
        npc_info = {
            "path": bp_path,
            "name": bp_name,
            "mesh": spawned_data.get("mesh"),
            "anim_class": spawned_data.get("anim_class"),
            "components": spawned_data.get("components", {}),
            "scs_components": scs_data.get("scs_components", {})
        }

        npc_data[bp_name] = npc_info

        # Summary
        comp_count = len(npc_info["components"]) + len(npc_info["scs_components"])
        debug_log(f"  Summary: Mesh={bool(npc_info['mesh'])}, AnimClass={bool(npc_info['anim_class'])}, Components={comp_count}")

    # Save to JSON
    debug_log("")
    debug_log("=" * 70)
    debug_log(f"SAVING TO {OUTPUT_FILE}")
    debug_log("=" * 70)

    with open(OUTPUT_FILE, 'w') as f:
        json.dump(npc_data, f, indent=2)

    debug_log(f"\nExtracted data for {len(npc_data)} NPCs")
    for name, data in npc_data.items():
        debug_log(f"  - {name}")

    return npc_data


if __name__ == "__main__":
    extract_npc_data()
