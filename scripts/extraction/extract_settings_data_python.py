"""
Extract Settings Widget data from bp_only using pure Python
This script works on the bp_only project which doesn't have SLFAutomationLibrary
"""
import unreal
import json
import os

CACHE_FILE = "C:/scripts/SLFConversion/migration_cache/settings_data.json"

def color_to_dict(color):
    """Convert FLinearColor to dict"""
    return {
        "R": color.r,
        "G": color.g,
        "B": color.b,
        "A": color.a
    }

def extract_settings_data():
    result = {
        "W_Settings_Entry": {},
        "W_Settings_CategoryEntry": {},
        "W_Settings": {},
        "PDA_CustomSettings": {}
    }

    # ─────────────────────────────────────────────────────────────────────────
    # Extract W_Settings_Entry
    # ─────────────────────────────────────────────────────────────────────────
    unreal.log_warning("Extracting W_Settings_Entry...")
    bp = unreal.load_asset("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry")
    if bp:
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                # Extract properties using get_editor_property
                try:
                    # Colors
                    unhovered = cdo.get_editor_property("UnhoveredColor")
                    if unhovered:
                        result["W_Settings_Entry"]["UnhoveredColor"] = color_to_dict(unhovered)
                        unreal.log_warning(f"  UnhoveredColor: {unhovered}")
                except:
                    pass

                try:
                    hovered = cdo.get_editor_property("HoveredColor")
                    if hovered:
                        result["W_Settings_Entry"]["HoveredColor"] = color_to_dict(hovered)
                        unreal.log_warning(f"  HoveredColor: {hovered}")
                except:
                    pass

                try:
                    entry_type = cdo.get_editor_property("EntryType")
                    result["W_Settings_Entry"]["EntryType"] = int(entry_type)
                    unreal.log_warning(f"  EntryType: {entry_type}")
                except:
                    pass

                try:
                    tag = cdo.get_editor_property("SettingTag")
                    if tag:
                        result["W_Settings_Entry"]["SettingTag"] = str(tag)
                        unreal.log_warning(f"  SettingTag: {tag}")
                except:
                    pass

                try:
                    name = cdo.get_editor_property("SettingName")
                    if name:
                        result["W_Settings_Entry"]["SettingName"] = str(name)
                        unreal.log_warning(f"  SettingName: {name}")
                except:
                    pass

                try:
                    desc = cdo.get_editor_property("SettingDescription")
                    if desc:
                        result["W_Settings_Entry"]["SettingDescription"] = str(desc)
                except:
                    pass

                try:
                    btn_text = cdo.get_editor_property("ButtonText (Single Button View)")
                    if btn_text:
                        result["W_Settings_Entry"]["ButtonText"] = str(btn_text)
                except:
                    pass

    # ─────────────────────────────────────────────────────────────────────────
    # Extract W_Settings_CategoryEntry
    # ─────────────────────────────────────────────────────────────────────────
    unreal.log_warning("Extracting W_Settings_CategoryEntry...")
    bp = unreal.load_asset("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry")
    if bp:
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                try:
                    cat = cdo.get_editor_property("SettingCategory")
                    if cat:
                        result["W_Settings_CategoryEntry"]["SettingCategory"] = str(cat)
                        unreal.log_warning(f"  SettingCategory: {cat}")
                except:
                    pass

                try:
                    icon = cdo.get_editor_property("Icon")
                    if icon:
                        path = str(icon.get_asset_path_name()) if hasattr(icon, 'get_asset_path_name') else str(icon)
                        result["W_Settings_CategoryEntry"]["Icon"] = path
                        unreal.log_warning(f"  Icon: {path}")
                except:
                    pass

                try:
                    idx = cdo.get_editor_property("SwitcherIndex")
                    result["W_Settings_CategoryEntry"]["SwitcherIndex"] = int(idx)
                    unreal.log_warning(f"  SwitcherIndex: {idx}")
                except:
                    pass

                try:
                    sel_color = cdo.get_editor_property("SelectedColor")
                    if sel_color:
                        result["W_Settings_CategoryEntry"]["SelectedColor"] = color_to_dict(sel_color)
                        unreal.log_warning(f"  SelectedColor: {sel_color}")
                except:
                    pass

                try:
                    desel_color = cdo.get_editor_property("DeselectedColor")
                    if desel_color:
                        result["W_Settings_CategoryEntry"]["DeselectedColor"] = color_to_dict(desel_color)
                        unreal.log_warning(f"  DeselectedColor: {desel_color}")
                except:
                    pass

    # ─────────────────────────────────────────────────────────────────────────
    # Extract W_Settings
    # ─────────────────────────────────────────────────────────────────────────
    unreal.log_warning("Extracting W_Settings...")
    bp = unreal.load_asset("/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings")
    if bp:
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                try:
                    cats = cdo.get_editor_property("CategoriesToHide")
                    if cats:
                        result["W_Settings"]["CategoriesToHide"] = [str(t) for t in cats]
                        unreal.log_warning(f"  CategoriesToHide: {len(cats)} tags")
                except:
                    pass

                try:
                    quit_desktop = cdo.get_editor_property("QuitToDesktop?")
                    result["W_Settings"]["QuitToDesktop?"] = bool(quit_desktop)
                    unreal.log_warning(f"  QuitToDesktop?: {quit_desktop}")
                except:
                    pass

    # ─────────────────────────────────────────────────────────────────────────
    # Extract PDA_CustomSettings
    # ─────────────────────────────────────────────────────────────────────────
    unreal.log_warning("Extracting PDA_CustomSettings...")
    bp = unreal.load_asset("/Game/SoulslikeFramework/Data/PDA_CustomSettings")
    if bp:
        gen_class = bp.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                try:
                    invert_x = cdo.get_editor_property("Invert Cam X")
                    result["PDA_CustomSettings"]["Invert Cam X"] = bool(invert_x)
                    unreal.log_warning(f"  Invert Cam X: {invert_x}")
                except:
                    pass

                try:
                    invert_y = cdo.get_editor_property("Invert Cam Y")
                    result["PDA_CustomSettings"]["Invert Cam Y"] = bool(invert_y)
                    unreal.log_warning(f"  Invert Cam Y: {invert_y}")
                except:
                    pass

                try:
                    cam_speed = cdo.get_editor_property("CamSpeed")
                    result["PDA_CustomSettings"]["CamSpeed"] = float(cam_speed)
                    unreal.log_warning(f"  CamSpeed: {cam_speed}")
                except:
                    pass

    # Save to file
    os.makedirs(os.path.dirname(CACHE_FILE), exist_ok=True)
    with open(CACHE_FILE, 'w') as f:
        json.dump(result, f, indent=2)

    unreal.log_warning(f"Saved to: {CACHE_FILE}")
    return result

if __name__ == "__main__":
    unreal.log_warning("=== EXTRACTING SETTINGS DATA (Python) ===")
    data = extract_settings_data()
    unreal.log_warning(f"Extracted {len(data)} widgets")
    unreal.log_warning("=== EXTRACTION COMPLETE ===")
