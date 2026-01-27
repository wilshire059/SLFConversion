# check_animbp_parent_v2.py
# Check AnimBP parent class and list all variables
import unreal

animbp_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

bp = unreal.load_asset(animbp_path)
if not bp:
    print("[ERROR] Could not load AnimBP")
else:
    gen_class = bp.generated_class()
    if gen_class:
        print(f"[INFO] Generated: {gen_class.get_name()}")

        parent = gen_class.get_super_class()
        if parent:
            print(f"[INFO] Parent: {parent.get_name()}")
            grandparent = parent.get_super_class()
            if grandparent:
                print(f"[INFO] Grandparent: {grandparent.get_name()}")

    # Check variables
    if hasattr(unreal, 'SLFAutomationLibrary'):
        variables = unreal.SLFAutomationLibrary.get_blueprint_variables(bp)
        print(f"[INFO] Variables count: {len(variables)}")
        # Show variables with ?
        for var in variables:
            if "?" in var:
                print(f"[VAR?] {var}")
            else:
                print(f"[VAR] {var}")
