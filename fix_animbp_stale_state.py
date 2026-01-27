# fix_animbp_stale_state.py
# Force recompile AnimBP to clear stale cached ErrorMsg entries

import unreal

ANIMBP_PATH = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"

def main():
    print("[Fix] Loading AnimBP...")

    bp = unreal.load_asset(ANIMBP_PATH)
    if not bp:
        print("[ERROR] Could not load AnimBP")
        return False

    print(f"[Fix] Loaded: {bp.get_name()}")
    print(f"[Fix] Blueprint class: {bp.get_class().get_name()}")

    # Get the generated class
    gen_class = bp.generated_class()
    if gen_class:
        print(f"[Fix] Generated class: {gen_class.get_name()}")

    # Force mark dirty and request compile
    print("[Fix] Marking asset dirty...")
    bp.modify(True)

    # Use BlueprintEditorLibrary to compile
    print("[Fix] Compiling Blueprint...")
    result = unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    print(f"[Fix] Compile result: {result}")

    # Save the asset
    print("[Fix] Saving asset...")
    unreal.EditorAssetLibrary.save_asset(ANIMBP_PATH)

    print("[Fix] Done! Please restart PIE to test.")
    return True

if __name__ == "__main__":
    main()
