"""
verify_animset_hierarchy.py
Verify and fix the weapon animset class hierarchy
"""
import unreal

def log(msg):
    unreal.log_warning(f"[HIERARCHY_CHECK] {msg}")

def main():
    log("=" * 70)
    log("VERIFYING WEAPON ANIMSET CLASS HIERARCHY")
    log("=" * 70)

    # 1. Load and check the C++ class
    log("\n=== C++ CLASS ===")
    cpp_class = unreal.load_class(None, "/Script/SLFConversion.PDA_WeaponAnimset")
    if cpp_class:
        log(f"C++ class: {cpp_class.get_name()}")
        log(f"C++ class path: {cpp_class.get_path_name()}")
    else:
        log("[ERROR] Could not load C++ class!")
        return

    # 2. Load and check the Blueprint
    log("\n=== BLUEPRINT ===")
    bp_path = "/Game/SoulslikeFramework/Data/WeaponAnimsets/PDA_WeaponAnimset"
    bp = unreal.load_asset(bp_path)
    if bp:
        log(f"Blueprint: {bp.get_name()}")
        log(f"Blueprint type: {type(bp).__name__}")

        # Get parent class via Blueprint's ParentClass property
        try:
            parent_class = bp.get_editor_property('parent_class')
            if parent_class:
                log(f"Blueprint ParentClass: {parent_class.get_name()}")
                log(f"Blueprint ParentClass path: {parent_class.get_path_name()}")

                # Check if parent is the C++ class
                if parent_class == cpp_class:
                    log("[OK] Blueprint is parented to C++ class")
                else:
                    log("[WARN] Blueprint parent does NOT match C++ class!")
            else:
                log("[ERROR] Blueprint has no ParentClass!")
        except Exception as e:
            log(f"Error getting parent_class: {e}")

        # Get generated class
        gen_class = None
        if hasattr(bp, 'generated_class') and callable(bp.generated_class):
            gen_class = bp.generated_class()
        elif hasattr(bp, 'GeneratedClass'):
            gen_class = bp.GeneratedClass

        if gen_class:
            log(f"\nGenerated class: {gen_class.get_name()}")
            log(f"Generated class path: {gen_class.get_path_name()}")
        else:
            log("[WARN] Could not get generated class")
    else:
        log(f"[ERROR] Could not load Blueprint: {bp_path}")
        return

    # 3. Check all animset data assets
    log("\n=== DATA ASSET INSTANCES ===")
    animset_paths = [
        "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_LightSword",
        "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Greatsword",
        "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Katana",
        "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Shield",
        "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_Unarmed",
    ]

    for path in animset_paths:
        animset = unreal.load_asset(path)
        if animset:
            animset_class = animset.get_class()
            class_path = animset_class.get_path_name()

            log(f"\n{animset.get_name()}:")
            log(f"  Class: {animset_class.get_name()}")
            log(f"  Class Path: {class_path}")

            # Check parent hierarchy using get_outer_most
            # Walk up the class hierarchy
            current = animset_class
            hierarchy = []
            max_depth = 10
            depth = 0
            while current and depth < max_depth:
                hierarchy.append(current.get_name())
                try:
                    # Use static_class to get parent
                    parent = None
                    # Check if this is a UClass with super class
                    if hasattr(current, 'static_class'):
                        # This is the class itself, not an instance
                        pass
                    depth += 1
                    # Can't easily walk hierarchy in Python, skip for now
                    break
                except:
                    break

            # The key check - is the data asset's class a child of C++ class?
            log(f"  Checking if instance is valid UPDA_WeaponAnimset...")

            # Try to get a C++ property that only exists on UPDA_WeaponAnimset
            try:
                # Try accessing a property that exists on UPDA_WeaponAnimset
                montage = animset.get_editor_property('one_h_light_combo_montage_r')
                log(f"  [OK] Can access one_h_light_combo_montage_r: {montage}")
            except Exception as e:
                log(f"  [ERROR] Cannot access C++ property: {e}")

            try:
                montage = animset.get_editor_property('two_h_light_combo_montage')
                log(f"  [OK] Can access two_h_light_combo_montage: {montage}")
            except Exception as e:
                log(f"  [ERROR] Cannot access C++ property: {e}")
        else:
            log(f"\n[ERROR] Could not load: {path}")

    # 4. Force recompile and resave the Blueprint
    log("\n=== RECOMPILING AND RESAVING ===")
    if bp:
        try:
            unreal.BlueprintEditorLibrary.compile_blueprint(bp)
            log("[OK] Blueprint compiled")
        except Exception as e:
            log(f"[ERROR] Compile failed: {e}")

        try:
            unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
            log("[OK] Blueprint saved")
        except Exception as e:
            log(f"[ERROR] Save failed: {e}")

    log("\n=== DONE ===")

if __name__ == "__main__":
    main()
