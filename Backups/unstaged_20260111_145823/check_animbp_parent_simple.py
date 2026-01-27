# check_animbp_parent_simple.py
# Simple check of AnimBP parent class
import unreal

def log(msg):
    print(msg)
    unreal.log_warning(f"[CHECK] {msg}")

def main():
    animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

    bp = unreal.load_asset(animbp_path)
    if not bp:
        log("[ERROR] Could not load AnimBP")
        return

    gen_class = bp.generated_class()
    if gen_class:
        log(f"Generated: {gen_class.get_name()}")

        parent = gen_class.get_super_class()
        if parent:
            log(f"Parent: {parent.get_name()}")
            grandparent = parent.get_super_class()
            if grandparent:
                log(f"Grandparent: {grandparent.get_name()}")

    # Check variables
    if hasattr(unreal, 'SLFAutomationLibrary'):
        variables = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
        log(f"Variables count: {len(variables)}")
        # Show first 10
        for var in variables[:10]:
            log(f"  Var: {var}")

if __name__ == "__main__":
    main()
