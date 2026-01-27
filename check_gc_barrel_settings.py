"""
Check GC_Barrel GeometryCollection asset physics settings.
The break thresholds and physics settings are stored in the asset.
"""

import unreal

def check_gc_barrel():
    """Check GC_Barrel asset settings."""

    gc_path = "/Game/SoulslikeFramework/Meshes/SM/Barrel/Destructible/GC_Barrel"

    output_lines = []
    def log(msg):
        output_lines.append(msg)
        print(msg)

    log("\n" + "="*80)
    log("GC_Barrel GeometryCollection Asset Settings")
    log("="*80)

    # Load the GeometryCollection asset
    gc_asset = unreal.load_asset(gc_path)
    if not gc_asset:
        log(f"ERROR: Could not load asset: {gc_path}")
        return

    log(f"\nAsset: {gc_asset.get_name()}")
    log(f"Class: {type(gc_asset).__name__}")

    # List all available properties
    log(f"\n--- Asset Properties ---")

    # Try to get various properties
    properties_to_check = [
        "simulate_physics",
        "enable_clustering",
        "cluster_group_index",
        "max_cluster_level",
        "damage_threshold",
        "collision_type",
        "implicit_type",
        "mass",
        "collision_particles_fraction",
        "initial_velocity_type",
        "initial_linear_velocity",
        "initial_angular_velocity",
        "physics_material",
        "object_type",
        "collision_group",
        "collision_mask",
        "damage_propagation_data",
        "size_specific_data",
    ]

    for prop_name in properties_to_check:
        try:
            value = gc_asset.get_editor_property(prop_name)
            log(f"  {prop_name}: {value}")
        except Exception as e:
            pass  # Property doesn't exist

    # Try to access specific damage/physics settings
    log(f"\n--- Damage Thresholds ---")
    try:
        # Try to get damage model
        damage_model = gc_asset.get_editor_property("damage_model")
        log(f"  DamageModel: {damage_model}")
    except Exception as e:
        log(f"  DamageModel: Not accessible ({e})")

    try:
        # Check if there's size-specific data with damage thresholds
        size_data = gc_asset.get_editor_property("size_specific_data")
        if size_data:
            log(f"  SizeSpecificData: {len(size_data)} entries")
            for i, entry in enumerate(size_data):
                log(f"    Entry {i}: {entry}")
    except Exception as e:
        log(f"  SizeSpecificData: Not accessible ({e})")

    # Check physics materials
    log(f"\n--- Physics Settings ---")
    try:
        phys_mat = gc_asset.get_editor_property("physical_material")
        log(f"  PhysicalMaterial: {phys_mat.get_path_name() if phys_mat else 'None'}")
    except:
        pass

    # Check root proxy data if available
    log(f"\n--- Export Text Analysis ---")
    try:
        export_text = unreal.EditorAssetLibrary.get_asset_export_text(gc_path)
        if export_text:
            lines_of_interest = []
            keywords = ['Damage', 'Threshold', 'Physics', 'Simulate', 'Cluster', 'Break', 'Mass', 'Collision']
            for line in export_text.split('\n'):
                line_stripped = line.strip()
                if any(kw.lower() in line_stripped.lower() for kw in keywords):
                    lines_of_interest.append(line_stripped)

            if lines_of_interest:
                log(f"  Found {len(lines_of_interest)} relevant lines:")
                for line in lines_of_interest[:30]:
                    log(f"    {line[:120]}")
    except Exception as e:
        log(f"  Export analysis error: {e}")

    log("\n" + "="*80)
    log("Check Complete")
    log("="*80 + "\n")

    # Write to file
    output_path = "C:/scripts/SLFConversion/migration_cache/gc_barrel_settings.txt"
    with open(output_path, "w") as f:
        f.write("\n".join(output_lines))
    log(f"Output written to: {output_path}")

if __name__ == "__main__":
    check_gc_barrel()
