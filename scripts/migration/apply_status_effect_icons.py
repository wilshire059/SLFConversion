"""
Apply status effect icon data to SLFConversion project.
Run on SLFConversion project after extracting from bp_only.
"""
import unreal
import json

INPUT_FILE = "C:/scripts/SLFConversion/migration_cache/status_effect_icons.json"

# Icon mappings - asset name to texture path
STATUS_EFFECT_ICONS = {
    "DA_StatusEffect_Burn": "/Game/SoulslikeFramework/Textures/StatusEffects/T_Burn.T_Burn",
    "DA_StatusEffect_Bleed": "/Game/SoulslikeFramework/Textures/StatusEffects/T_Bleed.T_Bleed",
    "DA_StatusEffect_Poison": "/Game/SoulslikeFramework/Textures/StatusEffects/T_Poison.T_Poison",
    "DA_StatusEffect_Frostbite": "/Game/SoulslikeFramework/Textures/StatusEffects/T_Frostbite.T_Frostbite",
    "DA_StatusEffect_Corruption": "/Game/SoulslikeFramework/Textures/StatusEffects/T_Corruption.T_Corruption",
    "DA_StatusEffect_Madness": "/Game/SoulslikeFramework/Textures/StatusEffects/T_Madness.T_Madness",
    "DA_StatusEffect_Plague": "/Engine/Functions/Engine_MaterialFunctions02/ExampleContent/Textures/testPattern1.testPattern1",
}

def apply_status_effect_icons():
    success_count = 0
    fail_count = 0

    for asset_name, icon_path in STATUS_EFFECT_ICONS.items():
        # Construct full asset path
        asset_path = f"/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/{asset_name}"

        asset = unreal.EditorAssetLibrary.load_asset(asset_path)
        if not asset:
            unreal.log_warning(f"Could not load: {asset_path}")
            fail_count += 1
            continue

        # Load the texture
        texture = unreal.EditorAssetLibrary.load_asset(icon_path)
        if not texture:
            unreal.log_warning(f"Could not load texture: {icon_path}")
            fail_count += 1
            continue

        # Set the icon property
        try:
            asset.set_editor_property("icon", texture)
            unreal.EditorAssetLibrary.save_asset(asset_path)
            unreal.log_warning(f"OK: {asset_name} -> {icon_path}")
            success_count += 1
        except Exception as e:
            unreal.log_warning(f"FAIL: {asset_name} - {e}")
            fail_count += 1

    unreal.log_warning(f"\nApplied icons: {success_count} succeeded, {fail_count} failed")

if __name__ == "__main__":
    apply_status_effect_icons()
