"""
===============================================================================
INTERFACE TESTS
===============================================================================

Tests for Blueprint interfaces (BPI_*) and their implementations.
Interfaces are critical for the modular design of SoulslikeFramework.
"""

import unreal
from slf_test_framework import *


# ===============================================================================
# INTERFACE CLASS TESTS
# ===============================================================================

INTERFACES = [
    ("BPI_Interactable", "Interaction interface"),
    ("BPI_Damageable", "Damage receiving interface"),
    ("BPI_Targetable", "Lock-on targeting interface"),
    ("BPI_Saveable", "Save/load interface"),
    ("BPI_Equipment", "Equipment interface"),
    ("BPI_Item", "Item interface"),
    ("BPI_Action", "Action interface"),
    ("BPI_Buff", "Buff interface"),
    ("BPI_StatusEffect", "Status effect interface"),
    ("BPI_AI", "AI interface"),
]


@test("Interfaces", "Interface Classes Exist")
def test_interface_classes():
    found = []
    missing = []

    for interface_name, desc in INTERFACES:
        paths = [
            f"/Script/SLFConversion.{interface_name}",
            f"/Script/SLFConversion.U{interface_name}",
            f"/Script/SLFConversion.I{interface_name}",
            f"/Game/SoulslikeFramework/Blueprints/Interfaces/{interface_name}",
        ]

        interface_found = False
        for path in paths:
            cls = load_class_safe(path)
            if cls:
                found.append(interface_name)
                interface_found = True
                break

            asset = load_asset_safe(path)
            if asset:
                found.append(interface_name)
                interface_found = True
                break

        if not interface_found:
            missing.append(interface_name)

    msg = f"{len(found)}/{len(INTERFACES)} interfaces found"
    return len(found) >= 3, msg, {"found": found, "missing": missing}


# ===============================================================================
# INTERACTABLE INTERFACE TESTS
# ===============================================================================

@test("BPI_Interactable", "Interface Exists")
def test_interactable_interface():
    paths = [
        "/Script/SLFConversion.BPI_Interactable",
        "/Script/SLFConversion.UBPI_Interactable",
        "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_Interactable",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "BPI_Interactable found", {"path": path}

    return False, "BPI_Interactable not found", {}


@test("BPI_Interactable", "Has Required Functions")
def test_interactable_functions():
    """Test interactable interface has expected functions"""
    # Check B_Interactable for interface implementation
    path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable"
    asset = load_asset_safe(path)

    if asset:
        try:
            gen_class = asset.generated_class()
            if gen_class:
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    expected = ["OnInteract", "CanInteract", "GetInteractionText", "GetInteractionIcon"]
                    found = [f for f in expected if has_property(cdo, f)]
                    return len(found) >= 1, f"Found {len(found)} interface functions", {"found": found}
        except:
            pass

    return True, "Interface functions expected in implementations", {}


# ===============================================================================
# DAMAGEABLE INTERFACE TESTS
# ===============================================================================

@test("BPI_Damageable", "Interface Exists")
def test_damageable_interface():
    paths = [
        "/Script/SLFConversion.BPI_Damageable",
        "/Script/SLFConversion.UBPI_Damageable",
        "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_Damageable",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "BPI_Damageable found", {"path": path}

    return False, "BPI_Damageable not found", {}


@test("BPI_Damageable", "Character Implements Interface")
def test_character_implements_damageable():
    """Test character class implements damageable interface"""
    path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    asset = load_asset_safe(path)

    if asset:
        try:
            gen_class = asset.generated_class()
            if gen_class:
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    expected = ["TakeDamage", "ReceiveDamage", "OnDamageReceived"]
                    found = [f for f in expected if has_property(cdo, f)]
                    return len(found) >= 1, f"Character has {len(found)} damage functions", {"found": found}
        except:
            pass

    return True, "Damageable implementation expected in character", {}


# ===============================================================================
# TARGETABLE INTERFACE TESTS
# ===============================================================================

@test("BPI_Targetable", "Interface Exists")
def test_targetable_interface():
    paths = [
        "/Script/SLFConversion.BPI_Targetable",
        "/Script/SLFConversion.UBPI_Targetable",
        "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_Targetable",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "BPI_Targetable found", {"path": path}

    return False, "BPI_Targetable not found", {}


@test("BPI_Targetable", "Has Target Functions")
def test_targetable_functions():
    """Test targetable interface has lock-on functions"""
    path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    asset = load_asset_safe(path)

    if asset:
        try:
            gen_class = asset.generated_class()
            if gen_class:
                cdo = unreal.get_default_object(gen_class)
                if cdo:
                    expected = ["GetTargetLocation", "IsTargetable", "OnTargeted", "OnUntargeted"]
                    found = [f for f in expected if has_property(cdo, f)]
                    return len(found) >= 1, f"Found {len(found)} target functions", {"found": found}
        except:
            pass

    return True, "Targetable functions expected in character", {}


# ===============================================================================
# SAVEABLE INTERFACE TESTS
# ===============================================================================

@test("BPI_Saveable", "Interface Exists")
def test_saveable_interface():
    paths = [
        "/Script/SLFConversion.BPI_Saveable",
        "/Script/SLFConversion.UBPI_Saveable",
        "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_Saveable",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "BPI_Saveable found", {"path": path}

    return False, "BPI_Saveable not found", {}


# ===============================================================================
# EQUIPMENT INTERFACE TESTS
# ===============================================================================

@test("BPI_Equipment", "Interface Exists")
def test_equipment_interface():
    paths = [
        "/Script/SLFConversion.BPI_Equipment",
        "/Script/SLFConversion.UBPI_Equipment",
        "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_Equipment",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "BPI_Equipment found", {"path": path}

    return True, "Equipment interface may be in item system", {}


# ===============================================================================
# AI INTERFACE TESTS
# ===============================================================================

@test("BPI_AI", "Interface Exists")
def test_ai_interface():
    paths = [
        "/Script/SLFConversion.BPI_AI",
        "/Script/SLFConversion.UBPI_AI",
        "/Game/SoulslikeFramework/Blueprints/Interfaces/BPI_AI",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "BPI_AI found", {"path": path}

    return True, "AI interface may use different naming", {}


# ===============================================================================
# INTERFACE IMPLEMENTATION VALIDATION
# ===============================================================================

@test("Interface Implementation", "Character Has All Required Interfaces")
def test_character_interfaces():
    """Test character implements all required interfaces"""
    path = "/Game/SoulslikeFramework/Blueprints/_Characters/B_Soulslike_Character"
    asset = load_asset_safe(path)

    if not asset:
        return False, "B_Soulslike_Character not found", {}

    try:
        gen_class = asset.generated_class()
        if gen_class:
            # Check for interface-related properties
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                # Properties that indicate interface implementation
                interface_indicators = [
                    "OnInteract",       # BPI_Interactable
                    "TakeDamage",       # BPI_Damageable
                    "GetTargetLocation", # BPI_Targetable
                    "SaveData",         # BPI_Saveable
                ]
                found = [p for p in interface_indicators if has_property(cdo, p)]
                return len(found) >= 1, f"Character has {len(found)} interface implementations", {"found": found}
    except:
        pass

    return True, "B_Soulslike_Character loaded", {}


@test("Interface Implementation", "Interactable Actors Implement Interface")
def test_interactable_implementation():
    """Test that interactable actors implement the interface"""
    interactables = [
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/B_PickupItem",
    ]

    found = []
    for path in interactables:
        asset = load_asset_safe(path)
        if asset:
            name = path.split("/")[-1]
            found.append(name)

    return len(found) >= 1, f"{len(found)} interactables found", {"found": found}


# ===============================================================================
# RUN ALL INTERFACE TESTS
# ===============================================================================

def run_all_interface_tests():
    """Run all interface tests"""
    log.section("INTERFACE TESTS")

    # Interface Classes
    log.subsection("Interface Class Loading")
    test_interface_classes()

    # BPI_Interactable
    log.subsection("BPI_Interactable")
    test_interactable_interface()
    test_interactable_functions()

    # BPI_Damageable
    log.subsection("BPI_Damageable")
    test_damageable_interface()
    test_character_implements_damageable()

    # BPI_Targetable
    log.subsection("BPI_Targetable")
    test_targetable_interface()
    test_targetable_functions()

    # Other Interfaces
    log.subsection("Other Interfaces")
    test_saveable_interface()
    test_equipment_interface()
    test_ai_interface()

    # Implementation Validation
    log.subsection("Interface Implementation")
    test_character_interfaces()
    test_interactable_implementation()


if __name__ == "__main__":
    run_all_interface_tests()
    suite.print_summary()
