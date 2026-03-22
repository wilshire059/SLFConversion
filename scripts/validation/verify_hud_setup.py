"""
Verify HUD Setup for SLFPlayerController
Checks that W_HUD can be loaded and SLFPlayerController is set up correctly
"""
import unreal

def log(msg):
    """Log using Unreal's logging system so it appears in output"""
    unreal.log_warning(msg)

def verify_hud_setup():
    log("=" * 60)
    log("HUD SETUP VERIFICATION")
    log("=" * 60)

    # Check if W_HUD class can be loaded
    hud_class_path = "/Game/SoulslikeFramework/Widgets/HUD/W_HUD.W_HUD_C"
    log(f"Loading W_HUD class: {hud_class_path}")

    hud_class = unreal.load_class(None, hud_class_path)
    if hud_class:
        log(f"[OK] W_HUD class loaded: {hud_class.get_name()}")
    else:
        log("[FAIL] Could not load W_HUD class")
        return False

    # Check if SLFPlayerController class exists
    pc_class_path = "/Script/SLFConversion.SLFPlayerController"
    log(f"Loading SLFPlayerController: {pc_class_path}")

    pc_class = unreal.load_class(None, pc_class_path)
    if pc_class:
        log(f"[OK] SLFPlayerController class loaded: {pc_class.get_name()}")
    else:
        log("[FAIL] Could not load SLFPlayerController class")
        return False

    # Check if W_GameMenu class can be loaded
    menu_class_path = "/Game/SoulslikeFramework/Widgets/HUD/W_GameMenu.W_GameMenu_C"
    log(f"Loading W_GameMenu class: {menu_class_path}")

    menu_class = unreal.load_class(None, menu_class_path)
    if menu_class:
        log(f"[OK] W_GameMenu class loaded: {menu_class.get_name()}")
    else:
        log("[WARN] Could not load W_GameMenu class (may have compile errors)")

    # Check PC_SoulslikeFramework Blueprint
    bp_pc_path = "/Game/SoulslikeFramework/Blueprints/Global/PC_SoulslikeFramework"
    log(f"Loading PC_SoulslikeFramework: {bp_pc_path}")

    bp_pc = unreal.load_asset(bp_pc_path)
    if bp_pc:
        log(f"[OK] PC_SoulslikeFramework Blueprint loaded")
        # Try to get parent class name from the generated class
        gen_class = bp_pc.generated_class()
        if gen_class:
            log(f"  Generated class: {gen_class.get_name()}")
            # Check parent using the class hierarchy
            parent_class = unreal.SystemLibrary.get_class_display_name(gen_class.get_class())
            log(f"  Class type: {parent_class}")
    else:
        log("[FAIL] Could not load PC_SoulslikeFramework Blueprint")

    # Check if IA_GameMenu input action exists
    ia_path = "/Game/SoulslikeFramework/Input/Actions/IA_GameMenu"
    log(f"Loading IA_GameMenu: {ia_path}")

    ia_asset = unreal.load_asset(ia_path)
    if ia_asset:
        log(f"[OK] IA_GameMenu input action loaded: {ia_asset.get_name()}")
    else:
        log("[WARN] Could not load IA_GameMenu - will try to find Tab in IMC")

    # Summary
    log("=" * 60)
    log("VERIFICATION SUMMARY:")
    log(f"  - W_HUD: {'OK' if hud_class else 'FAIL'}")
    log(f"  - SLFPlayerController: {'OK' if pc_class else 'FAIL'}")
    log(f"  - W_GameMenu: {'OK' if menu_class else 'WARN'}")
    log(f"  - IA_GameMenu: {'OK' if ia_asset else 'WARN'}")
    log("")
    log("Manual testing required:")
    log("1. Open editor, load L_Main_Showcase")
    log("2. Press Play (PIE)")
    log("3. Check Output Log for '[SLFPlayerController] Native_InitializeHUD'")
    log("4. Press Tab to toggle game menu")
    log("=" * 60)

    return True

# Run the verification
verify_hud_setup()
