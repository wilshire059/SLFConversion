"""
Check IMC_Gameplay mappings
"""
import unreal

imc = unreal.load_asset("/Game/SoulslikeFramework/Input/IMC_Gameplay.IMC_Gameplay")
crouch = unreal.load_asset("/Game/SoulslikeFramework/Input/Actions/IA_Crouch.IA_Crouch")

unreal.log_warning("=== IMC_Gameplay Mappings ===")
unreal.log_warning(f"IMC: {imc}")
unreal.log_warning(f"Crouch Action: {crouch}")

# Try map_key
if imc and crouch:
    try:
        mapping = imc.map_key(crouch, unreal.Keys.LEFT_CONTROL)
        unreal.log_warning(f"map_key returned: {mapping}")

        mapping2 = imc.map_key(crouch, unreal.Keys.C)
        unreal.log_warning(f"map_key C returned: {mapping2}")

        unreal.EditorAssetLibrary.save_asset("/Game/SoulslikeFramework/Input/IMC_Gameplay.IMC_Gameplay")
        unreal.log_warning("Saved IMC_Gameplay!")
    except Exception as e:
        unreal.log_error(f"map_key error: {e}")

unreal.log_warning("=== DONE ===")
