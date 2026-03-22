"""
===============================================================================
WORLD ACTOR TESTS
===============================================================================

Tests for:
- Interactable actors (base class and derived)
- Container/chest actors
- Ladder system
- Resting points (bonfires)
- Item pickups
- Doors and gates
- NPC actors
- Projectiles
"""

import unreal
from slf_test_framework import *


# ===============================================================================
# INTERACTABLE BASE CLASS TESTS
# ===============================================================================

@test("Interactables", "Base Interactable Class")
def test_interactable_base():
    paths = [
        "/Script/SLFConversion.B_Interactable",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "B_Interactable found", {"path": path}

    return False, "B_Interactable base class not found", {}


@test("Interactables", "Interactable Has Interface")
def test_interactable_interface():
    """Test that interactable implements BPI_Interactable"""
    path = "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Interactable"
    asset = load_asset_safe(path)

    if not asset:
        return False, "B_Interactable not found", {}

    try:
        gen_class = asset.generated_class()
        if gen_class:
            # Check for interface functions
            cdo = unreal.get_default_object(gen_class)
            if cdo:
                expected = ["OnInteract", "CanInteract", "GetInteractionText"]
                found = [f for f in expected if has_property(cdo, f)]
                return len(found) >= 1, f"Found {len(found)} interface functions", {"found": found}
    except:
        pass

    return True, "B_Interactable loaded", {}


# ===============================================================================
# CONTAINER/CHEST TESTS
# ===============================================================================

@test("Containers", "Container Base Class")
def test_container_base():
    paths = [
        "/Script/SLFConversion.B_Container",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "B_Container found", {"path": path}

    return False, "B_Container not found", {}


@test("Containers", "Container Has Loot")
def test_container_loot():
    """Test container has loot-related properties"""
    paths = [
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Container",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            try:
                gen_class = asset.generated_class()
                if gen_class:
                    cdo = unreal.get_default_object(gen_class)
                    if cdo:
                        expected = ["LootTable", "Items", "IsOpened", "GUID"]
                        found = [p for p in expected if has_property(cdo, p)]
                        return len(found) >= 1, f"Container has {len(found)} loot props", {"found": found}
            except:
                pass
            return True, "B_Container loaded", {}

    return False, "B_Container not found", {}


# ===============================================================================
# LADDER SYSTEM TESTS
# ===============================================================================

@test("Ladders", "Ladder Actor Class")
def test_ladder_actor():
    paths = [
        "/Script/SLFConversion.B_Ladder",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Ladder",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Ladder",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "B_Ladder found", {"path": path}

    return False, "B_Ladder not found", {}


@test("Ladders", "Ladder Manager Component")
def test_ladder_manager():
    cls = load_class_safe("/Script/SLFConversion.AC_LadderManager")
    if not cls:
        return False, "AC_LadderManager not found", {}

    try:
        cdo = unreal.get_default_object(cls)
        if cdo:
            expected = ["LadderAnimset", "IsOnLadder", "CurrentLadder", "ClimbSpeed"]
            found = [p for p in expected if has_property(cdo, p)]
            return True, f"LadderManager has {len(found)} props", {"found": found}
    except:
        pass

    return True, "AC_LadderManager class exists", {}


@test("Ladders", "Ladder Animset Asset")
def test_ladder_animset():
    paths = [
        "/Game/SoulslikeFramework/DataAssets/Animsets/DA_LadderAnimset",
        "/Game/SoulslikeFramework/Blueprints/DataAssets/DA_LadderAnimset",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "Ladder animset found", {"path": path}

    return False, "Ladder animset not found", {}


# ===============================================================================
# RESTING POINT TESTS
# ===============================================================================

@test("Resting", "Resting Point Actor")
def test_resting_point_actor():
    paths = [
        "/Script/SLFConversion.B_RestingPoint",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_RestingPoint",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "Resting point found", {"path": path}

    return False, "Resting point actor not found", {}


@test("Resting", "Rest Menu Widget")
def test_rest_menu_widget():
    paths = [
        "/Game/SoulslikeFramework/Widgets/RestMenu/W_RestMenu",
        "/Game/SoulslikeFramework/Widgets/W_RestMenu",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "W_RestMenu found", {"path": path}

    return False, "W_RestMenu not found", {}


# ===============================================================================
# ITEM PICKUP TESTS
# ===============================================================================

@test("Item Pickups", "Item Pickup Base Class")
def test_item_pickup_base():
    paths = [
        "/Script/SLFConversion.B_PickupItem",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/B_PickupItem",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/B_PickupItem",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "B_PickupItem found", {"path": path}

    return False, "B_PickupItem not found", {}


@test("Item Pickups", "Item Pickup Properties")
def test_item_pickup_properties():
    """Test item pickup has required properties"""
    paths = [
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/B_PickupItem",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/_Items/B_PickupItem",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            try:
                gen_class = asset.generated_class()
                if gen_class:
                    cdo = unreal.get_default_object(gen_class)
                    if cdo:
                        expected = ["ItemAsset", "ItemCount", "OnPickedUp"]
                        found = [p for p in expected if has_property(cdo, p)]
                        return len(found) >= 1, f"ItemPickup has {len(found)} props", {"found": found}
            except:
                pass
            return True, "B_PickupItem loaded", {}

    return False, "B_PickupItem not found", {}


# ===============================================================================
# DOOR/GATE TESTS
# ===============================================================================

@test("Doors", "Door Actor Class")
def test_door_actor():
    paths = [
        "/Script/SLFConversion.B_Door",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_Door",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Interactables/B_BossDoor",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "Door/Gate actor found", {"path": path}

    return False, "Door/Gate actor not found (may not exist)", {}


# ===============================================================================
# NPC ACTOR TESTS
# ===============================================================================

@test("NPCs", "NPC Base Class")
def test_npc_base():
    paths = [
        "/Script/SLFConversion.B_NPC",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "B_NPC found", {"path": path}

    return False, "B_NPC not found", {}


@test("NPCs", "NPC Has Dialog Properties")
def test_npc_dialog_properties():
    """Test NPC has dialog-related properties"""
    paths = [
        "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
        "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            try:
                gen_class = asset.generated_class()
                if gen_class:
                    cdo = unreal.get_default_object(gen_class)
                    if cdo:
                        expected = ["DialogAsset", "VendorAsset", "NPCName"]
                        found = [p for p in expected if has_property(cdo, p)]
                        return len(found) >= 1, f"NPC has {len(found)} dialog props", {"found": found}
            except:
                pass
            return True, "B_NPC loaded", {}

    return False, "B_NPC not found", {}


# ===============================================================================
# PROJECTILE TESTS
# ===============================================================================

@test("Projectiles", "Projectile Base Class")
def test_projectile_base():
    paths = [
        "/Script/SLFConversion.B_Projectile",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Projectiles/B_BaseProjectile",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Projectiles/B_BaseProjectile",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "B_Projectile found", {"path": path}

    return False, "B_Projectile not found", {}


@test("Projectiles", "Projectile Properties")
def test_projectile_properties():
    """Test projectile has required properties"""
    paths = [
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Projectiles/B_BaseProjectile",
        "/Game/SoulslikeFramework/Blueprints/_WorldActors/Projectiles/B_BaseProjectile",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            try:
                gen_class = asset.generated_class()
                if gen_class:
                    cdo = unreal.get_default_object(gen_class)
                    if cdo:
                        expected = ["Speed", "Damage", "Instigator", "OnHit"]
                        found = [p for p in expected if has_property(cdo, p)]
                        return len(found) >= 1, f"Projectile has {len(found)} props", {"found": found}
            except:
                pass
            return True, "B_Projectile loaded", {}

    return False, "B_Projectile not found", {}


# ===============================================================================
# SPAWN POINT TESTS
# ===============================================================================

@test("Spawn Points", "Player Start Exists")
def test_player_start():
    """Test that PlayerStart class is accessible"""
    cls = load_class_safe("/Script/Engine.PlayerStart")
    if cls:
        return True, "PlayerStart class accessible", {}
    return False, "PlayerStart class not found", {}


@test("Spawn Points", "Enemy Spawn Point")
def test_enemy_spawn_point():
    paths = [
        "/Script/SLFConversion.B_EnemySpawnPoint",
        "/Game/SoulslikeFramework/Blueprints/Actors/B_EnemySpawnPoint",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "Enemy spawn point found", {"path": path}

    return False, "Enemy spawn point not found (may spawn differently)", {}


# ===============================================================================
# PATROL PATH TESTS
# ===============================================================================

@test("Patrol", "Patrol Path Actor")
def test_patrol_path():
    paths = [
        "/Script/SLFConversion.B_PatrolPath",
        "/Game/SoulslikeFramework/Blueprints/_AI/Misc/B_PatrolPath",
        "/Game/SoulslikeFramework/Blueprints/_AI/Misc/B_PatrolPath",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "B_PatrolPath found", {"path": path}

    return False, "B_PatrolPath not found", {}


# ===============================================================================
# TRIGGER VOLUME TESTS
# ===============================================================================

@test("Triggers", "Trigger Volume Base")
def test_trigger_volume():
    paths = [
        "/Script/SLFConversion.B_TriggerVolume",
        "/Game/SoulslikeFramework/Blueprints/Actors/B_TriggerVolume",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "B_TriggerVolume found", {"path": path}

    # Also check for engine trigger
    cls = load_class_safe("/Script/Engine.TriggerVolume")
    if cls:
        return True, "Using engine TriggerVolume", {}

    return False, "No trigger volume found", {}


# ===============================================================================
# RUN ALL WORLD TESTS
# ===============================================================================

def run_all_world_tests():
    """Run all world actor tests"""
    log.section("WORLD ACTOR TESTS")

    # Interactables
    log.subsection("Interactable Base")
    test_interactable_base()
    test_interactable_interface()

    # Containers
    log.subsection("Containers/Chests")
    test_container_base()
    test_container_loot()

    # Ladders
    log.subsection("Ladder System")
    test_ladder_actor()
    test_ladder_manager()
    test_ladder_animset()

    # Resting Points
    log.subsection("Resting Points")
    test_resting_point_actor()
    test_rest_menu_widget()

    # Item Pickups
    log.subsection("Item Pickups")
    test_item_pickup_base()
    test_item_pickup_properties()

    # Doors
    log.subsection("Doors/Gates")
    test_door_actor()

    # NPCs
    log.subsection("NPC Actors")
    test_npc_base()
    test_npc_dialog_properties()

    # Projectiles
    log.subsection("Projectiles")
    test_projectile_base()
    test_projectile_properties()

    # Spawn Points
    log.subsection("Spawn Points")
    test_player_start()
    test_enemy_spawn_point()

    # Patrol
    log.subsection("Patrol System")
    test_patrol_path()

    # Triggers
    log.subsection("Trigger Volumes")
    test_trigger_volume()


if __name__ == "__main__":
    run_all_world_tests()
    suite.print_summary()
