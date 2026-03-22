"""
Compare B_Destructible SCS component settings between bp_only and SLFConversion.
Check for differences in component activation, physics settings, etc.
"""

import unreal

def compare_destructible():
    """Compare GC component settings in both projects (run on SLFConversion)."""

    bp_path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/LevelDesign/B_Destructible"

    output_lines = []
    def log(msg):
        output_lines.append(msg)
        print(msg)

    log("\n" + "="*80)
    log("B_Destructible SCS Component Comparison")
    log("="*80)

    # Load the Blueprint
    bp_asset = unreal.load_asset(bp_path)
    if not bp_asset:
        log(f"ERROR: Could not load Blueprint: {bp_path}")
        return

    # Get CDO
    gen_class = bp_asset.generated_class()
    if not gen_class:
        log("ERROR: No generated class!")
        return

    cdo = unreal.get_default_object(gen_class)
    log(f"\nBlueprint: {bp_asset.get_name()}")
    log(f"CDO: {cdo.get_name() if cdo else 'None'}")

    # Check CDO properties
    log(f"\n--- CDO Properties ---")
    try:
        gc = cdo.get_editor_property("geometry_collection")
        log(f"  GeometryCollection: {gc.get_path_name() if gc else 'None'}")
    except Exception as e:
        log(f"  GeometryCollection error: {e}")

    # Get all components on CDO
    log(f"\n--- Components on CDO ---")
    try:
        all_comps = cdo.get_components_by_class(unreal.ActorComponent)
        log(f"  Total components: {len(all_comps)}")
        for comp in all_comps:
            log(f"  - {comp.get_name()} ({type(comp).__name__})")

            if isinstance(comp, unreal.GeometryCollectionComponent):
                log(f"    Checking GeometryCollectionComponent settings:")

                # Check bAutoActivate
                try:
                    auto_activate = comp.get_editor_property("auto_activate")
                    log(f"      bAutoActivate: {auto_activate}")
                except Exception as e:
                    log(f"      bAutoActivate error: {e}")

                # Check visibility
                try:
                    visible = comp.get_editor_property("visible")
                    log(f"      bVisible: {visible}")
                except Exception as e:
                    log(f"      bVisible error: {e}")

                # Check RestCollection
                try:
                    rest_coll = comp.get_editor_property("rest_collection")
                    log(f"      RestCollection: {rest_coll.get_path_name() if rest_coll else 'None'}")
                except Exception as e:
                    log(f"      RestCollection error: {e}")

                # Check BodyInstance physics settings
                try:
                    body_instance = comp.get_editor_property("body_instance")
                    if body_instance:
                        try:
                            sim_physics = body_instance.get_editor_property("simulate_physics")
                            log(f"      BodyInstance.bSimulatePhysics: {sim_physics}")
                        except:
                            pass
                        try:
                            collision_enabled = body_instance.get_editor_property("collision_enabled")
                            log(f"      BodyInstance.CollisionEnabled: {collision_enabled}")
                        except:
                            pass
                except Exception as e:
                    log(f"      BodyInstance error: {e}")

                # Check EnableGravity
                try:
                    enable_gravity = comp.get_editor_property("enable_gravity")
                    log(f"      bEnableGravity: {enable_gravity}")
                except Exception as e:
                    log(f"      bEnableGravity error: {e}")

                # Check NotifyRigidBodyCollision
                try:
                    notify_collision = comp.get_editor_property("notify_rigid_body_collision")
                    log(f"      bNotifyRigidBodyCollision: {notify_collision}")
                except Exception as e:
                    log(f"      bNotifyRigidBodyCollision error: {e}")

                # Check collision object type
                try:
                    obj_type = comp.get_collision_object_type()
                    log(f"      CollisionObjectType: {obj_type}")
                except Exception as e:
                    log(f"      CollisionObjectType error: {e}")

    except Exception as e:
        log(f"  Component listing error: {e}")

    # Try to get SCS
    log(f"\n--- SCS Info ---")
    try:
        # Get the Blueprint's SCS via reflection
        bp = unreal.EditorAssetLibrary.load_blueprint_class(bp_path)
        log(f"  Blueprint class: {bp}")

        # Try to export Blueprint text for analysis
        export_text = unreal.EditorAssetLibrary.get_asset_export_text(bp_path)
        if export_text:
            # Look for GeometryCollectionComponent settings
            lines_of_interest = []
            for line in export_text.split('\n'):
                if 'GeometryCollection' in line or 'SimulatePhysics' in line or 'AutoActivate' in line:
                    lines_of_interest.append(line.strip())

            if lines_of_interest:
                log(f"  Relevant settings from export:")
                for line in lines_of_interest[:20]:  # First 20 matching lines
                    log(f"    {line}")
    except Exception as e:
        log(f"  SCS info error: {e}")

    log("\n" + "="*80)
    log("Comparison Complete")
    log("="*80 + "\n")

    # Write to file
    output_path = "C:/scripts/SLFConversion/migration_cache/destructible_scs_compare.txt"
    with open(output_path, "w") as f:
        f.write("\n".join(output_lines))
    log(f"Output written to: {output_path}")

if __name__ == "__main__":
    compare_destructible()
