"""
check_animset_class.py
Check the class hierarchy of animset data assets
"""
import unreal

def log(msg):
    unreal.log_warning(f"[CLASS_CHECK] {msg}")

def main():
    log("=" * 70)
    log("CHECKING ANIMSET CLASS HIERARCHY")
    log("=" * 70)

    animset_path = "/Game/SoulslikeFramework/Data/WeaponAnimsets/DA_Animset_LightSword"

    animset = unreal.load_asset(animset_path)
    if not animset:
        log(f"[ERROR] Could not load animset")
        return

    log(f"Asset: {animset.get_name()}")
    log(f"Class: {animset.get_class().get_name()}")
    log(f"Class Path: {animset.get_class().get_path_name()}")

    # Check parent chain
    current_class = animset.get_class()
    log(f"\nClass hierarchy:")
    depth = 0
    while current_class:
        log(f"  {'  ' * depth}{current_class.get_name()}")
        parent = current_class.get_super_class()
        if parent and parent.get_name() != current_class.get_name():
            current_class = parent
            depth += 1
        else:
            break

    # Check if it's our C++ class
    cpp_class = unreal.load_class(None, "/Script/SLFConversion.PDA_WeaponAnimset")
    if cpp_class:
        log(f"\nC++ UPDA_WeaponAnimset class: {cpp_class.get_name()}")
        log(f"C++ class path: {cpp_class.get_path_name()}")

        # Check if animset is instance of C++ class
        is_subclass = animset.get_class().is_child_of(cpp_class)
        log(f"Is animset a subclass of C++ UPDA_WeaponAnimset? {is_subclass}")
    else:
        log(f"\n[WARN] Could not load C++ UPDA_WeaponAnimset class")

    # Check Blueprint parent
    bp_path = "/Game/SoulslikeFramework/Data/WeaponAnimsets/PDA_WeaponAnimset"
    bp_class = unreal.load_asset(bp_path)
    if bp_class:
        log(f"\nBlueprint PDA_WeaponAnimset: {bp_class.get_name()}")
        if hasattr(bp_class, 'generated_class') and bp_class.generated_class():
            gen_class = bp_class.generated_class()
            log(f"Generated class: {gen_class.get_name()}")
            log(f"Generated class path: {gen_class.get_path_name()}")

            # Check parent of generated class
            parent = gen_class.get_super_class()
            if parent:
                log(f"Parent class: {parent.get_name()}")
                log(f"Parent class path: {parent.get_path_name()}")

if __name__ == "__main__":
    main()
