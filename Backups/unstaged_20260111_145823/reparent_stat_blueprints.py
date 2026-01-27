"""
Reparent Stat Blueprints to C++ Stat Classes

This script reparents all stat Blueprint assets to their corresponding C++ classes.
The stat classes (B_HP, B_Stamina, B_AP_Fire, etc.) need to inherit from their C++
equivalents (UB_HP, UB_Stamina, UB_AP_Fire, etc.) for the StatManagerComponent to
recognize them as UB_Stat subclasses.
"""

import unreal

# Mapping of Blueprint path -> C++ class path
# Format: (Blueprint asset path, C++ class path in /Script/SLFConversion)
STAT_REPARENT_MAP = {
    # Base class
    "/Game/SoulslikeFramework/Data/Stats/B_Stat": "/Script/SLFConversion.B_Stat",

    # Secondary stats
    "/Game/SoulslikeFramework/Data/Stats/Secondary/B_HP": "/Script/SLFConversion.B_HP",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/B_FP": "/Script/SLFConversion.B_FP",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/B_Stamina": "/Script/SLFConversion.B_Stamina",

    # Primary stats
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Vigor": "/Script/SLFConversion.B_Vigor",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Mind": "/Script/SLFConversion.B_Mind",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Endurance": "/Script/SLFConversion.B_Endurance",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Strength": "/Script/SLFConversion.B_Strength",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Dexterity": "/Script/SLFConversion.B_Dexterity",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Intelligence": "/Script/SLFConversion.B_Intelligence",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Faith": "/Script/SLFConversion.B_Faith",
    "/Game/SoulslikeFramework/Data/Stats/Primary/B_Arcane": "/Script/SLFConversion.B_Arcane",

    # Backend stats
    "/Game/SoulslikeFramework/Data/Stats/_Backend/B_Poise": "/Script/SLFConversion.B_Poise",
    "/Game/SoulslikeFramework/Data/Stats/_Backend/B_Stance": "/Script/SLFConversion.B_Stance",
    "/Game/SoulslikeFramework/Data/Stats/_Backend/B_IncantationPower": "/Script/SLFConversion.B_IncantationPower",
    "/Game/SoulslikeFramework/Data/Stats/_Backend/B_StatusEffectBuildup": "/Script/SLFConversion.B_StatusEffectBuildup",

    # Misc stats
    "/Game/SoulslikeFramework/Data/Stats/Misc/B_Weight": "/Script/SLFConversion.B_Weight",
    "/Game/SoulslikeFramework/Data/Stats/Misc/B_Discovery": "/Script/SLFConversion.B_Discovery",

    # Defense stats
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Focus": "/Script/SLFConversion.B_Resistance_Focus",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Immunity": "/Script/SLFConversion.B_Resistance_Immunity",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Robustness": "/Script/SLFConversion.B_Resistance_Robustness",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_Resistance_Vitality": "/Script/SLFConversion.B_Resistance_Vitality",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Physical": "/Script/SLFConversion.B_DN_Physical",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Fire": "/Script/SLFConversion.B_DN_Fire",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Frost": "/Script/SLFConversion.B_DN_Frost",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Lightning": "/Script/SLFConversion.B_DN_Lightning",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Magic": "/Script/SLFConversion.B_DN_Magic",
    "/Game/SoulslikeFramework/Data/Stats/Defense/B_DN_Holy": "/Script/SLFConversion.B_DN_Holy",

    # Attack Power stats
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Physical": "/Script/SLFConversion.B_AP_Physical",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Fire": "/Script/SLFConversion.B_AP_Fire",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Frost": "/Script/SLFConversion.B_AP_Frost",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Lightning": "/Script/SLFConversion.B_AP_Lightning",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Magic": "/Script/SLFConversion.B_AP_Magic",
    "/Game/SoulslikeFramework/Data/Stats/Secondary/AttackPower/B_AP_Holy": "/Script/SLFConversion.B_AP_Holy",

    # StatusEffect classes (base and derived)
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect": "/Script/SLFConversion.B_StatusEffect",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Bleed": "/Script/SLFConversion.B_StatusEffect_Bleed",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Burn": "/Script/SLFConversion.B_StatusEffect_Burn",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Corruption": "/Script/SLFConversion.B_StatusEffect_Corruption",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Frostbite": "/Script/SLFConversion.B_StatusEffect_Frostbite",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Madness": "/Script/SLFConversion.B_StatusEffect_Madness",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Plague": "/Script/SLFConversion.B_StatusEffect_Plague",
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect_Poison": "/Script/SLFConversion.B_StatusEffect_Poison",
}


def reparent_stat_blueprints():
    """Reparent all stat Blueprints to their C++ equivalents."""

    success_count = 0
    skip_count = 0
    error_count = 0

    unreal.log("=" * 60)
    unreal.log("REPARENTING STAT BLUEPRINTS TO C++ CLASSES")
    unreal.log("=" * 60)

    # Process base classes first, then children (order matters for inheritance)
    ordered_items = []

    # Base classes first
    base_paths = [
        "/Game/SoulslikeFramework/Data/Stats/B_Stat",
        "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectClasses/B_StatusEffect",
    ]

    for base_path in base_paths:
        if base_path in STAT_REPARENT_MAP:
            ordered_items.append((base_path, STAT_REPARENT_MAP[base_path]))

    # Then all others
    for bp_path, cpp_path in STAT_REPARENT_MAP.items():
        if bp_path not in base_paths:
            ordered_items.append((bp_path, cpp_path))

    for bp_path, cpp_path in ordered_items:
        # Load the Blueprint
        bp_asset = unreal.load_asset(bp_path)
        if not bp_asset:
            unreal.log_error(f"Failed to load Blueprint: {bp_path}")
            error_count += 1
            continue

        # Load the C++ class
        cpp_class = unreal.load_class(None, cpp_path)
        if not cpp_class:
            unreal.log_error(f"Failed to load C++ class: {cpp_path}")
            error_count += 1
            continue

        # Get current parent class name
        try:
            current_parent = bp_asset.get_editor_property('parent_class')
            current_parent_name = current_parent.get_name() if current_parent else "None"
        except:
            current_parent_name = "Unknown"

        # Check if already reparented to this C++ class
        cpp_class_name = cpp_path.split('.')[-1]
        if current_parent_name == cpp_class_name or current_parent_name == f"U{cpp_class_name}":
            unreal.log(f"SKIP: {bp_path} already has parent {current_parent_name}")
            skip_count += 1
            continue

        # Reparent using BlueprintEditorLibrary
        try:
            result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp_asset, cpp_class)
            if result:
                unreal.log(f"SUCCESS: Reparented {bp_path}")
                unreal.log(f"         {current_parent_name} -> {cpp_class_name}")

                # Save the asset
                unreal.EditorAssetLibrary.save_loaded_asset(bp_asset, False)
                success_count += 1
            else:
                unreal.log_error(f"FAILED: Could not reparent {bp_path}")
                error_count += 1
        except Exception as e:
            unreal.log_error(f"ERROR reparenting {bp_path}: {str(e)}")
            error_count += 1

    # Summary
    unreal.log("=" * 60)
    unreal.log("REPARENT SUMMARY")
    unreal.log("=" * 60)
    unreal.log(f"Success: {success_count}")
    unreal.log(f"Skipped: {skip_count}")
    unreal.log(f"Errors:  {error_count}")
    unreal.log(f"Total:   {len(STAT_REPARENT_MAP)}")
    unreal.log("=" * 60)

    return success_count, skip_count, error_count


if __name__ == "__main__":
    reparent_stat_blueprints()
