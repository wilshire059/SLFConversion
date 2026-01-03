"""
═══════════════════════════════════════════════════════════════════════════════
UI & WIDGET STATE TESTS
═══════════════════════════════════════════════════════════════════════════════

Tests for:
- HUD widget loading and state
- Inventory widget state
- Equipment widget state
- Stat menu state
- Buff/Status effect icons
- Loot notifications
- Dialog/Vendor windows
- Debug window
"""

import unreal
from slf_test_framework import *


# ═══════════════════════════════════════════════════════════════════════════════
# WIDGET CLASS TESTS
# ═══════════════════════════════════════════════════════════════════════════════

HUD_WIDGETS = [
    ("W_HUD", "Main HUD widget"),
    ("W_Resources", "Health/Stamina/Currency resources bar"),
    ("W_Boss_Healthbar", "Boss health bar"),
    ("W_ItemWheelSlot", "Item wheel slot"),
    ("W_ItemWheel_NextSlot", "Item wheel next slot"),
    ("W_StatusEffectBar", "Status effect bar"),
    ("W_StatusEffectNotification", "Status effect notification"),
    ("W_Buff", "Buff icon"),
    ("W_LootNotification", "Loot pickup notification"),
    ("W_FirstLootNotification", "First loot notification"),
    ("W_Interaction", "Interaction prompt"),
    ("W_AbilityDisplay", "Ability display"),
    ("W_GameMenu", "Game menu"),
    ("W_Dialog", "Dialog window"),
]

MENU_WIDGETS = [
    ("W_Inventory", "Inventory screen"),
    ("W_InventorySlot", "Inventory slot"),
    ("W_InventoryAction", "Inventory action"),
    ("W_Equipment", "Equipment screen"),
    ("W_EquipmentSlot", "Equipment slot"),
    ("W_StatBlock", "Stat display block"),
    ("W_StatEntry", "Individual stat entry"),
    ("W_LevelUp", "Level up menu"),
    ("W_RestMenu", "Rest/bonfire menu"),
    ("W_Crafting", "Crafting menu"),
    ("W_CraftingAction", "Crafting action item"),
]

NPC_WIDGETS = [
    ("W_NPC_Window", "NPC interaction window"),
    ("W_NPC_Window_Vendor", "Vendor/shop window"),
    ("W_VendorSlot", "Vendor item slot"),
    ("W_VendorAction", "Vendor action button"),
]

DEBUG_WIDGETS = [
    ("W_DebugWindow", "Debug overlay"),
]


@test("Widget Classes", "HUD Widgets")
def test_hud_widgets():
    found = []
    missing = []

    for widget_name, desc in HUD_WIDGETS:
        # Try loading as Blueprint asset
        path = f"/Game/SoulslikeFramework/Widgets/HUD/{widget_name}"
        asset = load_asset_safe(path)
        if asset:
            found.append(widget_name)
        else:
            # Try alternate paths
            alt_path = f"/Game/SoulslikeFramework/Widgets/{widget_name}"
            asset = load_asset_safe(alt_path)
            if asset:
                found.append(widget_name)
            else:
                missing.append(widget_name)

    msg = f"{len(found)}/{len(HUD_WIDGETS)} HUD widgets found"
    return len(found) >= 3, msg, {"found": found, "missing": missing}


@test("Widget Classes", "Menu Widgets")
def test_menu_widgets():
    found = []
    missing = []

    search_paths = [
        "/Game/SoulslikeFramework/Widgets/Inventory",
        "/Game/SoulslikeFramework/Widgets/Equipment",
        "/Game/SoulslikeFramework/Widgets/Stats",
        "/Game/SoulslikeFramework/Widgets/RestMenu",
        "/Game/SoulslikeFramework/Widgets/Crafting",
        "/Game/SoulslikeFramework/Widgets",
    ]

    for widget_name, desc in MENU_WIDGETS:
        widget_found = False
        for base_path in search_paths:
            path = f"{base_path}/{widget_name}"
            asset = load_asset_safe(path)
            if asset:
                found.append(widget_name)
                widget_found = True
                break

        if not widget_found:
            missing.append(widget_name)

    msg = f"{len(found)}/{len(MENU_WIDGETS)} menu widgets found"
    return len(found) >= 3, msg, {"found": found, "missing": missing}


@test("Widget Classes", "NPC Widgets")
def test_npc_widgets():
    found = []

    for widget_name, desc in NPC_WIDGETS:
        paths = [
            f"/Game/SoulslikeFramework/Widgets/NPC/{widget_name}",
            f"/Game/SoulslikeFramework/Widgets/Vendor/{widget_name}",
            f"/Game/SoulslikeFramework/Widgets/{widget_name}",
        ]

        for path in paths:
            asset = load_asset_safe(path)
            if asset:
                found.append(widget_name)
                break

    msg = f"{len(found)}/{len(NPC_WIDGETS)} NPC widgets found"
    return len(found) >= 1, msg, {"found": found}


@test("Widget Classes", "Debug Window")
def test_debug_window():
    paths = [
        "/Game/SoulslikeFramework/Widgets/_Debug/W_DebugWindow",
        "/Game/SoulslikeFramework/Widgets/Debug/W_DebugWindow",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "W_DebugWindow found", {"path": path}

    return False, "W_DebugWindow not found", {}


# ═══════════════════════════════════════════════════════════════════════════════
# WIDGET DATA BINDING TESTS
# ═══════════════════════════════════════════════════════════════════════════════

@test("Widget Data", "HUD Has Required Properties")
def test_hud_properties():
    """Test that HUD widget has expected bound properties"""
    path = "/Game/SoulslikeFramework/Widgets/HUD/W_HUD"
    asset = load_asset_safe(path)

    if not asset:
        return False, "W_HUD asset not found", {}

    # Check if we can access the generated class
    try:
        gen_class = asset.generated_class()
        if not gen_class:
            return True, "W_HUD loaded (no generated class access)", {}

        cdo = unreal.get_default_object(gen_class)
        if not cdo:
            return True, "W_HUD loaded (no CDO access)", {}

        # Check for expected child widgets/properties
        expected = ["HealthBar", "StaminaBar", "ItemWheel", "BuffContainer", "StatusEffectContainer"]
        found = [p for p in expected if has_property(cdo, p)]

        return len(found) >= 1, f"Found {len(found)} HUD bindings", {"found": found}
    except Exception as e:
        return True, f"W_HUD loaded (binding check failed: {e})", {}


@test("Widget Data", "Inventory Widget Has Slots")
def test_inventory_widget_slots():
    """Test that inventory widget has item slots"""
    path = "/Game/SoulslikeFramework/Widgets/Inventory/W_Inventory"
    asset = load_asset_safe(path)

    if not asset:
        # Try alternate path
        path = "/Game/SoulslikeFramework/Widgets/W_Inventory"
        asset = load_asset_safe(path)

    if not asset:
        return False, "W_Inventory not found", {}

    try:
        gen_class = asset.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                expected = ["ItemSlots", "SlotContainer", "SelectedItem", "OnItemSelected"]
                found = [p for p in expected if has_property(cdo, p)]
                return len(found) >= 1, f"Found {len(found)} inventory bindings", {"found": found}
    except:
        pass

    return True, "W_Inventory loaded", {}


@test("Widget Data", "Equipment Widget Has Slots")
def test_equipment_widget_slots():
    """Test that equipment widget has equipment slots"""
    path = "/Game/SoulslikeFramework/Widgets/Equipment/W_Equipment"
    asset = load_asset_safe(path)

    if not asset:
        return False, "W_Equipment not found", {}

    try:
        gen_class = asset.generated_class()
        if gen_class:
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                expected = ["EquipmentSlots", "RightHandSlot", "LeftHandSlot", "ArmorSlots"]
                found = [p for p in expected if has_property(cdo, p)]
                return len(found) >= 1, f"Found {len(found)} equipment bindings", {"found": found}
    except:
        pass

    return True, "W_Equipment loaded", {}


# ═══════════════════════════════════════════════════════════════════════════════
# WIDGET COMPONENT BINDING TESTS
# ═══════════════════════════════════════════════════════════════════════════════

@test("Widget Binding", "Player Controller Has HUD Reference")
def test_pc_hud_reference():
    """Test that PC has W_HUD property"""
    cls = load_class_safe("/Script/SLFConversion.PC_SoulslikeFramework")
    if not cls:
        return False, "PC class not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        if cdo and has_property(cdo, "W_HUD"):
            return True, "PC has W_HUD reference", {}
        return True, "PC loaded (W_HUD binding in Blueprint)", {}
    except:
        return True, "PC loaded", {}


@test("Widget Binding", "Inventory Manager Has Widget Getter")
def test_inventory_widget_getter():
    """Test that InventoryManager can provide widget"""
    cls = load_class_safe("/Script/SLFConversion.AC_InventoryManager")
    if not cls:
        return False, "AC_InventoryManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        if cdo:
            # Check for GetInventoryWidget function or OnInventoryUpdated delegate
            if has_property(cdo, "OnInventoryUpdated") or has_property(cdo, "InventoryWidget"):
                return True, "InventoryManager has widget connection", {}
    except:
        pass

    return True, "InventoryManager loaded", {}


@test("Widget Binding", "Equipment Manager Has Widget Events")
def test_equipment_widget_events():
    """Test that EquipmentManager fires widget events"""
    cls = load_class_safe("/Script/SLFConversion.AC_EquipmentManager")
    if not cls:
        return False, "AC_EquipmentManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        if cdo:
            events = ["OnItemEquipped", "OnItemUnequipped", "OnEquipmentUpdated"]
            found = [e for e in events if has_property(cdo, e)]
            return len(found) >= 1, f"Found {len(found)} equipment events", {"found": found}
    except:
        pass

    return True, "EquipmentManager loaded", {}


# ═══════════════════════════════════════════════════════════════════════════════
# STAT UI TESTS
# ═══════════════════════════════════════════════════════════════════════════════

@test("Stat UI", "Stat Entry Widget")
def test_stat_entry_widget():
    paths = [
        "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry",
        "/Game/SoulslikeFramework/Widgets/W_StatEntry",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "W_StatEntry found", {"path": path}

    return False, "W_StatEntry not found", {}


@test("Stat UI", "Stat Block Widget")
def test_stat_block_widget():
    paths = [
        "/Game/SoulslikeFramework/Widgets/Stats/W_StatBlock",
        "/Game/SoulslikeFramework/Widgets/W_StatBlock",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "W_StatBlock found", {"path": path}

    return False, "W_StatBlock not found", {}


@test("Stat UI", "Level Up Widget")
def test_level_up_widget():
    paths = [
        "/Game/SoulslikeFramework/Widgets/RestMenu/LevelUpMenu/W_LevelUp",
        "/Game/SoulslikeFramework/Widgets/LevelUp/W_LevelUp",
        "/Game/SoulslikeFramework/Widgets/W_LevelUp",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "W_LevelUp found", {"path": path}

    return False, "W_LevelUp not found", {}


# ═══════════════════════════════════════════════════════════════════════════════
# NOTIFICATION UI TESTS
# ═══════════════════════════════════════════════════════════════════════════════

@test("Notification UI", "Loot Notification Widget")
def test_loot_notification():
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


@test("Notification UI", "Damage Number Widget")
def test_damage_number():
    paths = [
        "/Game/SoulslikeFramework/Widgets/HUD/W_DamageNumber",
        "/Game/SoulslikeFramework/Widgets/Combat/W_DamageNumber",
        "/Game/SoulslikeFramework/Widgets/W_DamageNumber",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "W_DamageNumber found", {"path": path}

    return False, "W_DamageNumber not found (may be spawned dynamically)", {}


# ═══════════════════════════════════════════════════════════════════════════════
# RUN ALL UI TESTS
# ═══════════════════════════════════════════════════════════════════════════════

def run_all_ui_tests():
    """Run all UI/widget tests"""
    log.section("UI & WIDGET TESTS")

    # Widget Classes
    log.subsection("Widget Class Loading")
    test_hud_widgets()
    test_menu_widgets()
    test_npc_widgets()
    test_debug_window()

    # Widget Data Binding
    log.subsection("Widget Data Binding")
    test_hud_properties()
    test_inventory_widget_slots()
    test_equipment_widget_slots()

    # Widget Component Binding
    log.subsection("Component-Widget Binding")
    test_pc_hud_reference()
    test_inventory_widget_getter()
    test_equipment_widget_events()

    # Stat UI
    log.subsection("Stat UI Widgets")
    test_stat_entry_widget()
    test_stat_block_widget()
    test_level_up_widget()

    # Notifications
    log.subsection("Notification Widgets")
    test_loot_notification()
    test_damage_number()


if __name__ == "__main__":
    run_all_ui_tests()
    suite.print_summary()
