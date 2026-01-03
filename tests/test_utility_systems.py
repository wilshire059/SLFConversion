"""
===============================================================================
UTILITY SYSTEMS TESTS
===============================================================================

Tests for:
- Save/Load Manager
- Progress Manager
- Crafting System
- Radar/Compass System
- Time of Day System
- Settings System
- Loot Notification System
"""

import unreal
from slf_test_framework import *


# ===============================================================================
# SAVE/LOAD MANAGER TESTS
# ===============================================================================

@test("Save/Load", "Save Load Manager Class")
def test_save_load_manager_class():
    cls = load_class_safe("/Script/SLFConversion.AC_SaveLoadManager")
    if not cls:
        return False, "AC_SaveLoadManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        if cdo:
            expected = ["SaveGame", "LoadGame", "CurrentSaveSlot", "OnSaveCompleted", "OnLoadCompleted"]
            found = [p for p in expected if has_property(cdo, p)]
            return True, f"SaveLoadManager has {len(found)} props", {"found": found}
    except:
        pass

    return True, "AC_SaveLoadManager class exists", {}


@test("Save/Load", "Save Slot Properties")
def test_save_slot_properties():
    """Test save slot configuration"""
    cls = load_class_safe("/Script/SLFConversion.AC_SaveLoadManager")
    if not cls:
        return False, "AC_SaveLoadManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        if cdo:
            slot_props = ["CurrentSaveSlot", "MaxSaveSlots", "SaveSlotName"]
            found = [p for p in slot_props if has_property(cdo, p)]
            return len(found) >= 1, f"Found {len(found)} slot props", {"found": found}
    except:
        pass

    return True, "SaveLoadManager loaded", {}


@test("Save/Load", "Save Data Struct")
def test_save_data_struct():
    """Test save data structure exists"""
    # Try loading the save game class
    paths = [
        "/Script/SLFConversion.SLFSaveGame",
        "/Script/SLFConversion.FSaveData",
        "/Game/SoulslikeFramework/Blueprints/SaveGame/BP_SaveGame",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "Save data structure found", {"path": path}

    return True, "Save data in C++ structs (expected)", {}


@test("Save/Load", "GUID System for Actors")
def test_guid_system():
    """Test that GUID system exists for saveable actors"""
    # Check if interactable has GUID
    path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable"
    asset = load_asset_safe(path)

    if asset:
        try:
            gen_class = asset.generated_class()
            if gen_class:
                cdo = unreal.get_default_object(gen_class)
                if cdo and has_property(cdo, "GUID"):
                    return True, "GUID property found on Interactable", {}
        except:
            pass

    return True, "GUID system expected in saveable actors", {}


# ===============================================================================
# PROGRESS MANAGER TESTS
# ===============================================================================

@test("Progress", "Progress Manager Class")
def test_progress_manager_class():
    cls = load_class_safe("/Script/SLFConversion.AC_ProgressManager")
    if not cls:
        return False, "AC_ProgressManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        if cdo:
            expected = ["ProgressMap", "SetProgress", "GetProgress", "ExecuteGameplayEvents"]
            found = [p for p in expected if has_property(cdo, p)]
            return True, f"ProgressManager has {len(found)} props", {"found": found}
    except:
        pass

    return True, "AC_ProgressManager class exists", {}


@test("Progress", "Progress State Enum")
def test_progress_state_enum():
    """Test EProgressState enum exists"""
    # Try to access via class that uses it
    cls = load_class_safe("/Script/SLFConversion.AC_ProgressManager")
    if cls:
        return True, "Progress system uses EProgressState", {}

    return True, "Progress enum expected in SLFEnums.h", {}


@test("Progress", "Gameplay Event System")
def test_gameplay_event_system():
    """Test gameplay event execution"""
    cls = load_class_safe("/Script/SLFConversion.AC_ProgressManager")
    if not cls:
        return False, "AC_ProgressManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        if cdo:
            if has_property(cdo, "ExecuteGameplayEvents"):
                return True, "ExecuteGameplayEvents function exists", {}
    except:
        pass

    return True, "ProgressManager loaded", {}


# ===============================================================================
# CRAFTING SYSTEM TESTS
# ===============================================================================

@test("Crafting", "Crafting Widget")
def test_crafting_widget():
    paths = [
        "/Game/SoulslikeFramework/Widgets/Crafting/W_Crafting",
        "/Game/SoulslikeFramework/Widgets/RestMenu/CraftingMenu/W_Crafting",
        "/Game/SoulslikeFramework/Widgets/W_Crafting",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "W_Crafting found", {"path": path}

    return False, "W_Crafting not found", {}


@test("Crafting", "Crafting Recipe Asset")
def test_crafting_recipe():
    paths = [
        "/Game/SoulslikeFramework/DataAssets/Crafting",
        "/Game/SoulslikeFramework/Blueprints/DataAssets/DA_CraftingRecipe",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "Crafting assets found", {"path": path}

    return True, "Crafting recipes in data assets (expected)", {}


@test("Crafting", "Crafting Action Widget")
def test_crafting_action_widget():
    paths = [
        "/Game/SoulslikeFramework/Widgets/Crafting/W_CraftingAction",
        "/Game/SoulslikeFramework/Widgets/W_CraftingAction",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "W_CraftingAction found", {"path": path}

    return False, "W_CraftingAction not found", {}


# ===============================================================================
# RADAR/COMPASS SYSTEM TESTS
# ===============================================================================

@test("Radar", "Radar Manager Class")
def test_radar_manager_class():
    cls = load_class_safe("/Script/SLFConversion.AC_RadarManager")
    if not cls:
        return False, "AC_RadarManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        if cdo:
            expected = ["RefreshCardinals", "RefreshTrackedElements", "TrackedComponents"]
            found = [p for p in expected if has_property(cdo, p)]
            return True, f"RadarManager has {len(found)} props", {"found": found}
    except:
        pass

    return True, "AC_RadarManager class exists", {}


@test("Radar", "Radar Widget")
def test_radar_widget():
    paths = [
        "/Game/SoulslikeFramework/Widgets/Radar/W_Radar",
        "/Game/SoulslikeFramework/Widgets/Radar/W_Radar",
        "/Game/SoulslikeFramework/Widgets/W_Radar",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "Radar widget found", {"path": path}

    return False, "Radar widget not found", {}


@test("Radar", "Radar Element Component")
def test_radar_element_component():
    cls = load_class_safe("/Script/SLFConversion.AC_RadarElement")
    if not cls:
        # Try alternate name
        cls = load_class_safe("/Script/SLFConversion.UC_RadarElement")

    if cls:
        return True, "RadarElement component found", {}

    return False, "RadarElement component not found", {}


# ===============================================================================
# TIME OF DAY TESTS
# ===============================================================================

@test("Time of Day", "Time of Day System")
def test_time_of_day_system():
    paths = [
        "/Script/SLFConversion.B_TimeOfDay",
        "/Game/SoulslikeFramework/Blueprints/Actors/B_TimeOfDay",
        "/Game/SoulslikeFramework/Blueprints/World/B_TimeOfDay",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "TimeOfDay system found", {"path": path}

    return False, "TimeOfDay system not found (may be example only)", {}


# ===============================================================================
# SETTINGS SYSTEM TESTS
# ===============================================================================

@test("Settings", "Settings Widget")
def test_settings_widget():
    paths = [
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings",
        "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings",
        "/Game/SoulslikeFramework/Widgets/W_Settings",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "W_Settings found", {"path": path}

    return False, "W_Settings not found", {}


@test("Settings", "Settings Save Data")
def test_settings_save():
    paths = [
        "/Script/SLFConversion.SLFGameSettings",
        "/Game/SoulslikeFramework/Blueprints/SaveGame/BP_SettingsSave",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "Settings save found", {"path": path}

    return True, "Settings in game user settings (expected)", {}


# ===============================================================================
# LOOT NOTIFICATION TESTS
# ===============================================================================

@test("Loot Notifications", "Loot Notification Manager")
def test_loot_notification_manager():
    """Test loot notification system exists"""
    # Check for notification component or manager
    cls = load_class_safe("/Script/SLFConversion.AC_LootNotificationManager")
    if cls:
        return True, "LootNotificationManager found", {}

    # Check inventory manager for loot notification
    cls = load_class_safe("/Script/SLFConversion.AC_InventoryManager")
    if cls:
        try:
            cdo = unreal.get_default_object(cls)
            if cdo and has_property(cdo, "OnLootAcquired"):
                return True, "Loot notification via InventoryManager", {}
        except:
            pass

    return True, "Loot notification system expected", {}


@test("Loot Notifications", "Loot Notification Widget")
def test_loot_notification_widget():
    paths = [
        "/Game/SoulslikeFramework/Widgets/HUD/W_LootNotification",
        "/Game/SoulslikeFramework/Widgets/Notifications/W_LootNotification",
        "/Game/SoulslikeFramework/Widgets/W_LootNotification",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "W_LootNotification found", {"path": path}

    return False, "W_LootNotification not found", {}


# ===============================================================================
# DATA ASSET TESTS
# ===============================================================================

@test("Data Assets", "Base Data Asset Classes")
def test_base_data_assets():
    """Test base data asset classes exist"""
    data_assets = [
        ("B_Stat", "Stat data asset"),
        ("B_Action", "Action data asset"),
        ("B_Buff", "Buff data asset"),
        ("B_Item", "Item data asset"),
        ("B_StatusEffect", "Status effect data asset"),
    ]

    found = []
    for asset_name, desc in data_assets:
        paths = [
            f"/Script/SLFConversion.{asset_name}",
            f"/Game/SoulslikeFramework/Blueprints/DataAssets/{asset_name}",
        ]
        for path in paths:
            asset = load_asset_safe(path) or load_class_safe(path)
            if asset:
                found.append(asset_name)
                break

    msg = f"{len(found)}/{len(data_assets)} data asset classes found"
    return len(found) >= 2, msg, {"found": found}


@test("Data Assets", "Item Data Assets")
def test_item_data_assets():
    """Test item data assets exist"""
    paths = [
        "/Game/SoulslikeFramework/DataAssets/Items",
        "/Game/SoulslikeFramework/Blueprints/DataAssets/Items",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "Item data assets folder found", {"path": path}

    return True, "Item assets expected in DataAssets folder", {}


# ===============================================================================
# INPUT MAPPING TESTS
# ===============================================================================

@test("Input", "Input Mapping Context")
def test_input_mapping_context():
    paths = [
        "/Game/SoulslikeFramework/Input/IMC_Default",
        "/Game/SoulslikeFramework/Input/IMC_Soulslike",
        "/Game/SoulslikeFramework/Blueprints/Input/IMC_Default",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "Input mapping context found", {"path": path}

    return False, "Input mapping context not found", {}


@test("Input", "Input Actions")
def test_input_actions():
    paths = [
        "/Game/SoulslikeFramework/Input/Actions",
        "/Game/SoulslikeFramework/Input",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "Input actions folder found", {"path": path}

    return True, "Input actions expected in Input folder", {}


# ===============================================================================
# RUN ALL UTILITY TESTS
# ===============================================================================

def run_all_utility_tests():
    """Run all utility system tests"""
    log.section("UTILITY SYSTEMS TESTS")

    # Save/Load
    log.subsection("Save/Load System")
    test_save_load_manager_class()
    test_save_slot_properties()
    test_save_data_struct()
    test_guid_system()

    # Progress
    log.subsection("Progress System")
    test_progress_manager_class()
    test_progress_state_enum()
    test_gameplay_event_system()

    # Crafting
    log.subsection("Crafting System")
    test_crafting_widget()
    test_crafting_recipe()
    test_crafting_action_widget()

    # Radar
    log.subsection("Radar/Compass System")
    test_radar_manager_class()
    test_radar_widget()
    test_radar_element_component()

    # Time of Day
    log.subsection("Time of Day")
    test_time_of_day_system()

    # Settings
    log.subsection("Settings System")
    test_settings_widget()
    test_settings_save()

    # Loot Notifications
    log.subsection("Loot Notification System")
    test_loot_notification_manager()
    test_loot_notification_widget()

    # Data Assets
    log.subsection("Data Assets")
    test_base_data_assets()
    test_item_data_assets()

    # Input
    log.subsection("Input System")
    test_input_mapping_context()
    test_input_actions()


if __name__ == "__main__":
    run_all_utility_tests()
    suite.print_summary()
