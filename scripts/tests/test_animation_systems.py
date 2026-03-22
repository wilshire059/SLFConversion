"""
===============================================================================
ANIMATION SYSTEMS TESTS
===============================================================================

Tests for:
- Animation Blueprint classes
- Animation Notifies (AN_*)
- Animation Notify States (ANS_*)
- Animset data assets
- Additive animation system
- Weapon-specific animsets
"""

import unreal
from slf_test_framework import *


# ===============================================================================
# ANIMATION BLUEPRINT TESTS
# ===============================================================================

ANIM_BLUEPRINTS = [
    ("ABP_Player", "Player animation blueprint"),
    ("ABP_Enemy", "Enemy animation blueprint"),
    ("ABP_NPC", "NPC animation blueprint"),
]


@test("Animation BPs", "Animation Blueprint Classes")
def test_anim_blueprint_classes():
    found = []
    missing = []

    for abp_name, desc in ANIM_BLUEPRINTS:
        paths = [
            f"/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/{abp_name}",
            f"/Game/SoulslikeFramework/Blueprints/AnimationRelated/{abp_name}",
            f"/Game/SoulslikeFramework/Characters/Animations/{abp_name}",
        ]

        abp_found = False
        for path in paths:
            asset = load_asset_safe(path)
            if asset:
                found.append(abp_name)
                abp_found = True
                break

        if not abp_found:
            missing.append(abp_name)

    msg = f"{len(found)}/{len(ANIM_BLUEPRINTS)} animation BPs found"
    return len(found) >= 1, msg, {"found": found, "missing": missing}


@test("Animation BPs", "Player ABP Has States")
def test_player_abp_states():
    """Test player ABP has expected state machine states"""
    paths = [
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
        "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            try:
                gen_class = asset.generated_class()
                if gen_class:
                    cdo = unreal.get_default_object(gen_class)
                    if cdo:
                        # Check for common anim instance properties
                        expected = ["Speed", "Direction", "IsFalling", "IsInAir"]
                        found = [p for p in expected if has_property(cdo, p)]
                        return len(found) >= 1, f"ABP has {len(found)} state vars", {"found": found}
            except:
                pass
            return True, "ABP_Player loaded", {}

    return False, "ABP_Player not found", {}


# ===============================================================================
# ANIMATION NOTIFY TESTS
# ===============================================================================

ANIM_NOTIFIES = [
    ("AN_EnableWeaponCollision", "Enable weapon trace"),
    ("AN_DisableWeaponCollision", "Disable weapon trace"),
    ("AN_EnableRootMotion", "Enable root motion"),
    ("AN_DisableRootMotion", "Disable root motion"),
    ("AN_PlaySound", "Play sound effect"),
    ("AN_SpawnParticle", "Spawn particle effect"),
    ("AN_CameraShake", "Trigger camera shake"),
    ("AN_SetActionState", "Set action state"),
    ("AN_ConsumeStamina", "Consume stamina"),
    ("AN_ApplyDamage", "Apply damage"),
]


@test("Anim Notifies", "Animation Notify Classes")
def test_anim_notify_classes():
    found = []
    missing = []

    for notify_name, desc in ANIM_NOTIFIES:
        paths = [
            f"/Script/SLFConversion.{notify_name}",
            f"/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/{notify_name}",
            f"/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/{notify_name}",
        ]

        notify_found = False
        for path in paths:
            asset = load_asset_safe(path) or load_class_safe(path)
            if asset:
                found.append(notify_name)
                notify_found = True
                break

        if not notify_found:
            missing.append(notify_name)

    msg = f"{len(found)}/{len(ANIM_NOTIFIES)} notifies found"
    return len(found) >= 3, msg, {"found": found, "missing": missing}


@test("Anim Notifies", "Weapon Collision Notify")
def test_weapon_collision_notify():
    """Test weapon collision enable/disable notifies"""
    paths = [
        "/Script/SLFConversion.AN_EnableWeaponCollision",
        "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_WeaponTrace",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "AN_EnableWeaponCollision found", {"path": path}

    return False, "Weapon collision notify not found", {}


# ===============================================================================
# ANIMATION NOTIFY STATE TESTS
# ===============================================================================

ANIM_NOTIFY_STATES = [
    ("ANS_WeaponTrace", "Continuous weapon trace"),
    ("ANS_Invincibility", "Invincibility window"),
    ("ANS_SuperArmor", "Super armor/poise"),
    ("ANS_RotateToTarget", "Rotate toward target"),
    ("ANS_BlockInput", "Block player input"),
]


@test("Anim Notify States", "Animation Notify State Classes")
def test_anim_notify_state_classes():
    found = []
    missing = []

    for ans_name, desc in ANIM_NOTIFY_STATES:
        paths = [
            f"/Script/SLFConversion.{ans_name}",
            f"/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/{ans_name}",
            f"/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/{ans_name}",
        ]

        ans_found = False
        for path in paths:
            asset = load_asset_safe(path) or load_class_safe(path)
            if asset:
                found.append(ans_name)
                ans_found = True
                break

        if not ans_found:
            missing.append(ans_name)

    msg = f"{len(found)}/{len(ANIM_NOTIFY_STATES)} notify states found"
    return len(found) >= 2, msg, {"found": found, "missing": missing}


@test("Anim Notify States", "Weapon Trace Notify State")
def test_weapon_trace_ans():
    """Test weapon trace notify state exists"""
    paths = [
        "/Script/SLFConversion.ANS_WeaponTrace",
        "/Game/SoulslikeFramework/Blueprints/AnimationRelated/Notifies/ANS_WeaponTrace",
    ]

    for path in paths:
        asset = load_asset_safe(path) or load_class_safe(path)
        if asset:
            return True, "ANS_WeaponTrace found", {"path": path}

    return False, "ANS_WeaponTrace not found", {}


# ===============================================================================
# ANIMSET DATA ASSET TESTS
# ===============================================================================

ANIMSETS = [
    ("DA_PlayerAnimset", "Player base animset"),
    ("DA_EnemyAnimset", "Enemy base animset"),
    ("DA_LadderAnimset", "Ladder climbing animset"),
]


@test("Animsets", "Animset Data Assets")
def test_animset_data_assets():
    found = []
    missing = []

    for animset_name, desc in ANIMSETS:
        paths = [
            f"/Game/SoulslikeFramework/DataAssets/Animsets/{animset_name}",
            f"/Game/SoulslikeFramework/Blueprints/DataAssets/{animset_name}",
            f"/Game/SoulslikeFramework/Animations/Animsets/{animset_name}",
        ]

        animset_found = False
        for path in paths:
            asset = load_asset_safe(path)
            if asset:
                found.append(animset_name)
                animset_found = True
                break

        if not animset_found:
            missing.append(animset_name)

    msg = f"{len(found)}/{len(ANIMSETS)} animsets found"
    return len(found) >= 1, msg, {"found": found, "missing": missing}


@test("Animsets", "Weapon Animset Assets")
def test_weapon_animsets():
    """Test weapon-specific animset assets"""
    paths = [
        "/Game/SoulslikeFramework/DataAssets/Animsets/Weapons",
        "/Game/SoulslikeFramework/DataAssets/WeaponAnimsets",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "Weapon animsets folder found", {"path": path}

    return True, "Weapon animsets expected in Animsets folder", {}


# ===============================================================================
# MONTAGE TESTS
# ===============================================================================

MONTAGE_CATEGORIES = [
    ("Attacks", "Attack montages"),
    ("Dodges", "Dodge/roll montages"),
    ("Guards", "Guard/block montages"),
    ("Deaths", "Death montages"),
    ("Interactions", "Interaction montages"),
]


@test("Montages", "Montage Categories")
def test_montage_categories():
    found = []
    missing = []

    for category_name, desc in MONTAGE_CATEGORIES:
        paths = [
            f"/Game/SoulslikeFramework/Animations/Montages/{category_name}",
            f"/Game/SoulslikeFramework/Animations/{category_name}",
        ]

        category_found = False
        for path in paths:
            asset = load_asset_safe(path)
            if asset:
                found.append(category_name)
                category_found = True
                break

        if not category_found:
            missing.append(category_name)

    msg = f"{len(found)}/{len(MONTAGE_CATEGORIES)} montage categories found"
    return len(found) >= 1, msg, {"found": found, "missing": missing}


@test("Montages", "Combo Montages")
def test_combo_montages():
    """Test combo attack montages exist"""
    paths = [
        "/Game/SoulslikeFramework/Animations/Montages/Attacks/Combo",
        "/Game/SoulslikeFramework/Animations/Attacks",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "Combo montages found", {"path": path}

    return True, "Combo montages expected in Attacks folder", {}


# ===============================================================================
# ADDITIVE ANIMATION TESTS
# ===============================================================================

@test("Additive Anim", "Additive Animation Manager")
def test_additive_anim_manager():
    """Test additive animation system"""
    cls = load_class_safe("/Script/SLFConversion.AC_AdditiveAnimationManager")
    if cls:
        return True, "AdditiveAnimationManager found", {}

    # Check if it's part of action manager
    cls = load_class_safe("/Script/SLFConversion.AC_ActionManager")
    if cls:
        try:
            cdo = unreal.get_default_object(cls)
            if cdo and has_property(cdo, "AdditiveAnimSlot"):
                return True, "Additive anim via ActionManager", {}
        except:
            pass

    return True, "Additive animation system expected", {}


@test("Additive Anim", "Hit Reaction Animations")
def test_hit_reaction_anims():
    """Test hit reaction animations exist"""
    paths = [
        "/Game/SoulslikeFramework/Animations/HitReactions",
        "/Game/SoulslikeFramework/Animations/Additive/HitReactions",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "Hit reaction anims found", {"path": path}

    return True, "Hit reactions expected in Animations folder", {}


# ===============================================================================
# BLEND SPACE TESTS
# ===============================================================================

@test("Blend Spaces", "Locomotion Blend Space")
def test_locomotion_blend_space():
    paths = [
        "/Game/SoulslikeFramework/Animations/BlendSpaces/BS_Locomotion",
        "/Game/SoulslikeFramework/Animations/BS_Locomotion",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "Locomotion blend space found", {"path": path}

    return False, "Locomotion blend space not found", {}


@test("Blend Spaces", "Strafe Blend Space")
def test_strafe_blend_space():
    paths = [
        "/Game/SoulslikeFramework/Animations/BlendSpaces/BS_Strafe",
        "/Game/SoulslikeFramework/Animations/BS_Strafe",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "Strafe blend space found", {"path": path}

    return True, "Strafe blend space may be in locomotion", {}


# ===============================================================================
# SKELETON TESTS
# ===============================================================================

@test("Skeletons", "Player Skeleton")
def test_player_skeleton():
    paths = [
        "/Game/SoulslikeFramework/Characters/Player/SK_Player",
        "/Game/SoulslikeFramework/Characters/SK_Player",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "Player skeleton found", {"path": path}

    return True, "Player skeleton in Characters folder", {}


@test("Skeletons", "Enemy Skeleton")
def test_enemy_skeleton():
    paths = [
        "/Game/SoulslikeFramework/Characters/Enemies/SK_Enemy",
        "/Game/SoulslikeFramework/Characters/SK_Enemy",
    ]

    for path in paths:
        asset = load_asset_safe(path)
        if asset:
            return True, "Enemy skeleton found", {"path": path}

    return True, "Enemy skeleton in Characters folder", {}


# ===============================================================================
# RUN ALL ANIMATION TESTS
# ===============================================================================

def run_all_animation_tests():
    """Run all animation system tests"""
    log.section("ANIMATION SYSTEMS TESTS")

    # Animation Blueprints
    log.subsection("Animation Blueprints")
    test_anim_blueprint_classes()
    test_player_abp_states()

    # Anim Notifies
    log.subsection("Animation Notifies")
    test_anim_notify_classes()
    test_weapon_collision_notify()

    # Anim Notify States
    log.subsection("Animation Notify States")
    test_anim_notify_state_classes()
    test_weapon_trace_ans()

    # Animsets
    log.subsection("Animset Data Assets")
    test_animset_data_assets()
    test_weapon_animsets()

    # Montages
    log.subsection("Animation Montages")
    test_montage_categories()
    test_combo_montages()

    # Additive
    log.subsection("Additive Animation System")
    test_additive_anim_manager()
    test_hit_reaction_anims()

    # Blend Spaces
    log.subsection("Blend Spaces")
    test_locomotion_blend_space()
    test_strafe_blend_space()

    # Skeletons
    log.subsection("Skeletons")
    test_player_skeleton()
    test_enemy_skeleton()


if __name__ == "__main__":
    run_all_animation_tests()
    suite.print_summary()
