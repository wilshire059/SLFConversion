"""
Extract status effect icon data from backup (bp_only) for migration.
Run this on bp_only project first, then apply to SLFConversion.
"""
import unreal
import json

OUTPUT_FILE = "C:/scripts/SLFConversion/migration_cache/status_effect_icons.json"

def extract_status_effect_icons():
    output = {}

    status_effect_paths = [
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Burn",
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Bleed",
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Poison",
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Frostbite",
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Corruption",
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Madness",
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Plague",
    ]

    for path in status_effect_paths:
        asset = unreal.EditorAssetLibrary.load_asset(path)
        if not asset:
            unreal.log_warning(f"Could not load: {path}")
            continue

        asset_name = asset.get_name()
        data = {}

        # Try to get Icon via editor property
        try:
            icon = asset.get_editor_property("icon")
            if icon:
                data["icon"] = icon.get_path_name()
                unreal.log_warning(f"{asset_name}: Icon = {icon.get_path_name()}")
            else:
                data["icon"] = None
                unreal.log_warning(f"{asset_name}: Icon = (None)")
        except Exception as e:
            unreal.log_warning(f"{asset_name}: Icon error - {e}")
            data["icon"] = None

        # Get BarFillColor
        try:
            color = asset.get_editor_property("bar_fill_color")
            data["bar_fill_color"] = {
                "r": color.r,
                "g": color.g,
                "b": color.b,
                "a": color.a
            }
            unreal.log_warning(f"{asset_name}: BarFillColor = ({color.r:.2f}, {color.g:.2f}, {color.b:.2f})")
        except Exception as e:
            unreal.log_warning(f"{asset_name}: BarFillColor error - {e}")
            data["bar_fill_color"] = None

        # Get Tag
        try:
            tag = asset.get_editor_property("tag")
            if tag:
                data["tag"] = tag.tag_name.to_string() if hasattr(tag, 'tag_name') else str(tag)
            else:
                data["tag"] = None
        except Exception as e:
            data["tag"] = None

        # Get TriggeredText
        try:
            triggered_text = asset.get_editor_property("triggered_text")
            data["triggered_text"] = str(triggered_text) if triggered_text else None
        except:
            data["triggered_text"] = None

        output[asset_name] = data

    # Write to JSON
    with open(OUTPUT_FILE, 'w') as f:
        json.dump(output, f, indent=2)

    unreal.log_warning(f"\nExtracted {len(output)} status effects to {OUTPUT_FILE}")

if __name__ == "__main__":
    extract_status_effect_icons()
