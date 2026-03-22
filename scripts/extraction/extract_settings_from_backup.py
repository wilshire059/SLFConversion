"""
Extract settings widget data from bp_only backup.
This script should be run on the bp_only project.
"""
import unreal
import json
import os

output_path = "C:/scripts/SLFConversion/migration_cache/settings_data.json"

unreal.log_warning("=== EXTRACTING SETTINGS WIDGET DATA FROM BP_ONLY ===\n")

data = {}

def get_linear_color(obj, prop_name):
    """Extract FLinearColor as dict."""
    try:
        color = obj.get_editor_property(prop_name)
        return {"R": color.r, "G": color.g, "B": color.b, "A": color.a}
    except:
        return None

def get_gameplay_tag(obj, prop_name):
    """Extract FGameplayTag as string."""
    try:
        tag = obj.get_editor_property(prop_name)
        return tag.to_string() if tag else ""
    except:
        return ""

# ═══════════════════════════════════════════════════════════════════════════
# W_Settings_Entry
# ═══════════════════════════════════════════════════════════════════════════
entry_bp = unreal.load_asset("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry")
if entry_bp and entry_bp.generated_class():
    cdo = unreal.get_default_object(entry_bp.generated_class())
    if cdo:
        unreal.log_warning(f"Found W_Settings_Entry CDO: {cdo.get_class().get_name()}")

        entry_data = {}

        # Colors
        entry_data["UnhoveredColor"] = get_linear_color(cdo, "UnhoveredColor")
        entry_data["HoveredColor"] = get_linear_color(cdo, "HoveredColor")

        # Text properties
        try:
            entry_data["SettingName"] = str(cdo.get_editor_property("SettingName"))
        except:
            entry_data["SettingName"] = "Setting Entry"

        try:
            entry_data["SettingDescription"] = str(cdo.get_editor_property("SettingDescription"))
        except:
            entry_data["SettingDescription"] = "Setting Entry"

        try:
            entry_data["ButtonText"] = str(cdo.get_editor_property("ButtonText"))
        except:
            entry_data["ButtonText"] = "Button Text"

        # GameplayTag - try different approaches
        try:
            tag = cdo.get_editor_property("SettingTag")
            if hasattr(tag, 'tag_name'):
                entry_data["SettingTag"] = tag.tag_name.to_string()
            else:
                entry_data["SettingTag"] = str(tag) if tag else ""
        except Exception as e:
            entry_data["SettingTag"] = ""
            unreal.log_warning(f"  SettingTag extraction error: {e}")

        # EntryType enum
        try:
            entry_type = cdo.get_editor_property("EntryType")
            entry_data["EntryType"] = str(entry_type) if entry_type else "Default"
        except:
            entry_data["EntryType"] = "Default"

        data["W_Settings_Entry"] = entry_data
        unreal.log_warning(f"  Extracted: {entry_data}")
else:
    unreal.log_warning("W_Settings_Entry not found!")

# ═══════════════════════════════════════════════════════════════════════════
# W_Settings_CategoryEntry
# ═══════════════════════════════════════════════════════════════════════════
cat_bp = unreal.load_asset("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry")
if cat_bp and cat_bp.generated_class():
    cdo = unreal.get_default_object(cat_bp.generated_class())
    if cdo:
        unreal.log_warning(f"\nFound W_Settings_CategoryEntry CDO: {cdo.get_class().get_name()}")

        cat_data = {}

        # Colors
        cat_data["SelectedColor"] = get_linear_color(cdo, "SelectedColor")
        cat_data["DeselectedColor"] = get_linear_color(cdo, "DeselectedColor")

        # SwitcherIndex
        try:
            cat_data["SwitcherIndex"] = cdo.get_editor_property("SwitcherIndex")
        except:
            cat_data["SwitcherIndex"] = 0

        # GameplayTag
        try:
            tag = cdo.get_editor_property("SettingCategory")
            if hasattr(tag, 'tag_name'):
                cat_data["SettingCategory"] = tag.tag_name.to_string()
            else:
                cat_data["SettingCategory"] = str(tag) if tag else ""
        except:
            cat_data["SettingCategory"] = ""

        data["W_Settings_CategoryEntry"] = cat_data
        unreal.log_warning(f"  Extracted: {cat_data}")
else:
    unreal.log_warning("W_Settings_CategoryEntry not found!")

# ═══════════════════════════════════════════════════════════════════════════
# W_Settings
# ═══════════════════════════════════════════════════════════════════════════
settings_bp = unreal.load_asset("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings")
if settings_bp and settings_bp.generated_class():
    cdo = unreal.get_default_object(settings_bp.generated_class())
    if cdo:
        unreal.log_warning(f"\nFound W_Settings CDO: {cdo.get_class().get_name()}")

        settings_data = {}

        # Try to get QuitToDesktop with ? suffix
        try:
            settings_data["QuitToDesktop"] = cdo.get_editor_property("QuitToDesktop?")
        except:
            try:
                settings_data["QuitToDesktop"] = cdo.get_editor_property("QuitToDesktop")
            except:
                settings_data["QuitToDesktop"] = False

        data["W_Settings"] = settings_data
        unreal.log_warning(f"  Extracted: {settings_data}")
else:
    unreal.log_warning("W_Settings not found!")

# ═══════════════════════════════════════════════════════════════════════════
# PDA_CustomSettings
# ═══════════════════════════════════════════════════════════════════════════
pda_bp = unreal.load_asset("/Game/SoulslikeFramework/Data/PDA_CustomSettings")
if pda_bp and pda_bp.generated_class():
    cdo = unreal.get_default_object(pda_bp.generated_class())
    if cdo:
        unreal.log_warning(f"\nFound PDA_CustomSettings CDO: {cdo.get_class().get_name()}")

        pda_data = {}

        try:
            pda_data["InvertCamX"] = cdo.get_editor_property("Invert Cam X")
        except:
            try:
                pda_data["InvertCamX"] = cdo.get_editor_property("InvertCamX")
            except:
                pda_data["InvertCamX"] = False

        try:
            pda_data["InvertCamY"] = cdo.get_editor_property("Invert Cam Y")
        except:
            try:
                pda_data["InvertCamY"] = cdo.get_editor_property("InvertCamY")
            except:
                pda_data["InvertCamY"] = False

        try:
            pda_data["CamSpeed"] = cdo.get_editor_property("CamSpeed")
        except:
            pda_data["CamSpeed"] = 1.0

        data["PDA_CustomSettings"] = pda_data
        unreal.log_warning(f"  Extracted: {pda_data}")
else:
    unreal.log_warning("PDA_CustomSettings not found!")

# Save to JSON
os.makedirs(os.path.dirname(output_path), exist_ok=True)
with open(output_path, 'w') as f:
    json.dump(data, f, indent=2, default=str)

unreal.log_warning(f"\n=== SAVED TO {output_path} ===")
