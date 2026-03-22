# Verify migration results
import unreal

output = []

def log(msg):
    output.append(msg)
    unreal.log_warning("VERIFY: " + msg)

log("=" * 60)
log("MIGRATION VERIFICATION")
log("=" * 60)

# 1. Check PC_SoulslikeFramework parent class
try:
    pc = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Blueprints/Global/PC_SoulslikeFramework")
    if pc:
        bp_class = pc.get_class()
        # Get parent class name from the class path
        parent_path = str(bp_class.get_path_name())
        log("1. PC_SoulslikeFramework class path: " + parent_path)

        # Check if it inherits from SLFPlayerController by checking the generated class name
        gen_class = pc.generated_class() if hasattr(pc, 'generated_class') else None
        if gen_class:
            parent = gen_class.get_super_class() if hasattr(gen_class, 'get_super_class') else None
            parent_name = parent.get_name() if parent else "Unknown"
            log("   Parent class: " + parent_name)
            if "SLFPlayerController" in parent_name:
                log("   [OK] Reparented to SLFPlayerController")
            else:
                log("   [FAIL] Not reparented correctly")
        else:
            log("   [INFO] Could not determine parent class")
    else:
        log("1. [FAIL] Could not load PC_SoulslikeFramework")
except Exception as e:
    log("1. [ERROR] " + str(e))

# 2. Check Dodge Montages
try:
    dodge_asset = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge")
    if dodge_asset:
        dodge_montages = dodge_asset.get_editor_property('DodgeMontages')
        forward = getattr(dodge_montages, 'forward', None)
        backstep = getattr(dodge_montages, 'backstep', None)
        log("2. Dodge Montages:")
        log("   Forward: " + (forward.get_name() if forward else "NULL"))
        log("   Backstep: " + (backstep.get_name() if backstep else "NULL"))
        if forward and backstep:
            log("   [OK] Dodge montages are set")
        else:
            log("   [FAIL] Dodge montages are NULL")
    else:
        log("2. [FAIL] Could not load DA_Action_Dodge")
except Exception as e:
    log("2. [ERROR] " + str(e))

# 3. Check Tab key binding
try:
    imc = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Input/IMC_Gameplay")
    ia_game_menu = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Input/Actions/IA_GameMenu")
    if imc and ia_game_menu:
        key_mappings_data = imc.get_editor_property('default_key_mappings')
        mappings = key_mappings_data.get_editor_property('mappings')

        found_keys = []
        for m in mappings:
            action = m.get_editor_property('action')
            if action and action.get_path_name() == ia_game_menu.get_path_name():
                key = m.get_editor_property('key')
                key_name = key.get_editor_property('key_name')
                found_keys.append(str(key_name))

        log("3. IA_GameMenu keys: " + str(found_keys))
        if any("Tab" in k for k in found_keys):
            log("   [OK] Tab key is bound")
        else:
            log("   [FAIL] Tab key not found")
    else:
        log("3. [FAIL] Could not load IMC_Gameplay or IA_GameMenu")
except Exception as e:
    log("3. [ERROR] " + str(e))

log("=" * 60)

# Write to file
with open("C:/scripts/SLFConversion/verify_result.txt", "w") as f:
    f.write("\n".join(output))
