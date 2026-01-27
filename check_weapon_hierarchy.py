# check_weapon_hierarchy.py
# Check the class hierarchy of weapons in SLFConversion

import unreal

def log(msg):
    unreal.log_warning(str(msg))

BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/B_Item_Weapon",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_SwordExample01",
    "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/Weapons/Player/B_Item_Weapon_Shield",
]

def check_hierarchy():
    log("=" * 70)
    log("WEAPON CLASS HIERARCHY CHECK")
    log("=" * 70)

    for bp_path in BLUEPRINTS:
        bp_name = bp_path.split("/")[-1]
        log(f"\n=== {bp_name} ===")

        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            log(f"  ERROR: Could not load")
            continue

        # Get parent class
        try:
            parent_class = bp.get_editor_property("parent_class")
            if parent_class:
                log(f"  Parent class: {parent_class.get_name()}")
                log(f"  Parent path: {parent_class.get_path_name()}")

                # Check if parent has specific properties
                try:
                    parent_cdo = unreal.get_default_object(parent_class)
                    if parent_cdo:
                        # Check for C++ weapon properties
                        has_cpp_props = False
                        try:
                            parent_cdo.get_editor_property("default_mesh_relative_location")
                            has_cpp_props = True
                        except:
                            pass
                        log(f"  Parent has C++ weapon props: {has_cpp_props}")
                except:
                    pass
        except Exception as e:
            log(f"  Parent class ERROR: {e}")

        # Get generated class and check full hierarchy
        gen_class = bp.generated_class()
        if gen_class:
            log(f"  Generated class: {gen_class.get_name()}")

            # Walk up the class hierarchy
            current = gen_class
            depth = 0
            log(f"  Class hierarchy:")
            while current and depth < 10:
                log(f"    [{depth}] {current.get_name()}")
                current = current.get_super_class()
                depth += 1

    log("\n" + "=" * 70)

if __name__ == "__main__":
    check_hierarchy()
