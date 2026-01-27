# apply_statuseffect_rankinfo.py
# Restore status effect RankInfo data that was lost during migration
# The FInstancedStruct data (RelevantData) containing damage info was cleared during reparenting

import unreal
import json

# Status effect data to apply
# Extracted from BlueprintDNA/DataAsset/DA_StatusEffect_*.json
STATUS_EFFECT_DATA = {
    "DA_StatusEffect_Poison": {
        "TriggeredText": "POISONED",
        "ranks": {
            1: {
                "type": "FStatusEffectTick",
                "Duration": 5.0,
                "Interval": 1.0,
                "TickingStatAdjustment": [
                    {"StatTag": "SoulslikeFramework.Stat.Secondary.HP", "MinAmount": -5.0, "MaxAmount": -10.0}
                ]
            },
            2: {
                "type": "FStatusEffectStatChanges",
                "StatChanges": [
                    {"StatTag": "SoulslikeFramework.Stat.Secondary.HP", "MinAmount": -50.0, "MaxAmount": -100.0}
                ]
            },
            3: {
                "type": "FStatusEffectOneShotAndTick",
                "Duration": 10.0,
                "Interval": 1.0,
                "InstantStatAdjustment": [
                    {"StatTag": "SoulslikeFramework.Stat.Secondary.HP", "MinAmount": -20.0, "MaxAmount": -40.0}
                ],
                "TickingStatAdjustment": [
                    {"StatTag": "SoulslikeFramework.Stat.Secondary.HP", "MinAmount": -5.0, "MaxAmount": -10.0}
                ]
            }
        }
    },
    "DA_StatusEffect_Bleed": {
        "TriggeredText": "HEMORRHAGING",
        "ranks": {
            1: {
                "type": "FStatusEffectStatChanges",
                "StatChanges": [
                    {"StatTag": "SoulslikeFramework.Stat.Secondary.HP", "MinAmount": -50.0, "MaxAmount": -75.0}
                ]
            },
            2: {
                "type": "FStatusEffectStatChanges",
                "StatChanges": [
                    {"StatTag": "SoulslikeFramework.Stat.Secondary.HP", "MinAmount": -100.0, "MaxAmount": -150.0}
                ]
            },
            3: {
                "type": "FStatusEffectOneShotAndTick",
                "Duration": 8.0,
                "Interval": 1.0,
                "InstantStatAdjustment": [
                    {"StatTag": "SoulslikeFramework.Stat.Secondary.HP", "MinAmount": -150.0, "MaxAmount": -250.0}
                ],
                "TickingStatAdjustment": [
                    {"StatTag": "SoulslikeFramework.Stat.Secondary.HP", "MinAmount": -15.0, "MaxAmount": -25.0}
                ]
            }
        }
    },
    "DA_StatusEffect_Burn": {
        "TriggeredText": "SCORCHED",
        "ranks": {
            1: {
                "type": "FStatusEffectTick",
                "Duration": 4.0,
                "Interval": 0.5,
                "TickingStatAdjustment": [
                    {"StatTag": "SoulslikeFramework.Stat.Secondary.HP", "MinAmount": -8.0, "MaxAmount": -12.0}
                ]
            },
            2: {
                "type": "FStatusEffectTick",
                "Duration": 6.0,
                "Interval": 0.5,
                "TickingStatAdjustment": [
                    {"StatTag": "SoulslikeFramework.Stat.Secondary.HP", "MinAmount": -12.0, "MaxAmount": -18.0}
                ]
            },
            3: {
                "type": "FStatusEffectOneShotAndTick",
                "Duration": 8.0,
                "Interval": 0.5,
                "InstantStatAdjustment": [
                    {"StatTag": "SoulslikeFramework.Stat.Secondary.HP", "MinAmount": -50.0, "MaxAmount": -100.0}
                ],
                "TickingStatAdjustment": [
                    {"StatTag": "SoulslikeFramework.Stat.Secondary.HP", "MinAmount": -15.0, "MaxAmount": -25.0}
                ]
            }
        }
    },
    "DA_StatusEffect_Frostbite": {
        "TriggeredText": "FROSTBITTEN",
        "ranks": {
            1: {
                "type": "FStatusEffectStatChanges",
                "StatChanges": [
                    {"StatTag": "SoulslikeFramework.Stat.Secondary.HP", "MinAmount": -30.0, "MaxAmount": -50.0},
                    {"StatTag": "SoulslikeFramework.Stat.Secondary.Stamina", "MinAmount": -20.0, "MaxAmount": -30.0}
                ]
            },
            2: {
                "type": "FStatusEffectStatChanges",
                "StatChanges": [
                    {"StatTag": "SoulslikeFramework.Stat.Secondary.HP", "MinAmount": -60.0, "MaxAmount": -100.0},
                    {"StatTag": "SoulslikeFramework.Stat.Secondary.Stamina", "MinAmount": -40.0, "MaxAmount": -60.0}
                ]
            },
            3: {
                "type": "FStatusEffectOneShotAndTick",
                "Duration": 6.0,
                "Interval": 1.0,
                "InstantStatAdjustment": [
                    {"StatTag": "SoulslikeFramework.Stat.Secondary.HP", "MinAmount": -100.0, "MaxAmount": -150.0}
                ],
                "TickingStatAdjustment": [
                    {"StatTag": "SoulslikeFramework.Stat.Secondary.Stamina", "MinAmount": -10.0, "MaxAmount": -15.0}
                ]
            }
        }
    }
}

def create_stat_change(stat_tag_str, min_amount, max_amount, value_type="CurrentValue", try_activate_regen=False):
    """Create an FStatChange struct"""
    stat_change = unreal.StatChange()
    stat_change.set_editor_property("stat_tag", unreal.GameplayTag.request_gameplay_tag(stat_tag_str))
    stat_change.set_editor_property("min_amount", min_amount)
    stat_change.set_editor_property("max_amount", max_amount)
    # value_type enum
    if value_type == "CurrentValue":
        stat_change.set_editor_property("value_type", unreal.SLFValueType.CURRENT_VALUE)
    else:
        stat_change.set_editor_property("value_type", unreal.SLFValueType.MAX_VALUE)
    stat_change.set_editor_property("b_try_activate_regen", try_activate_regen)
    return stat_change

def create_status_effect_tick(duration, interval, ticking_adjustments):
    """Create an FStatusEffectTick struct"""
    tick_struct = unreal.StatusEffectTick()
    tick_struct.set_editor_property("duration", duration)
    tick_struct.set_editor_property("interval", interval)

    ticking_array = []
    for adj in ticking_adjustments:
        stat_change = create_stat_change(adj["StatTag"], adj["MinAmount"], adj["MaxAmount"])
        ticking_array.append(stat_change)
    tick_struct.set_editor_property("ticking_stat_adjustment", ticking_array)

    return tick_struct

def create_status_effect_stat_changes(stat_changes):
    """Create an FStatusEffectStatChanges struct"""
    changes_struct = unreal.StatusEffectStatChanges()

    changes_array = []
    for change in stat_changes:
        stat_change = create_stat_change(change["StatTag"], change["MinAmount"], change["MaxAmount"])
        changes_array.append(stat_change)
    changes_struct.set_editor_property("stat_changes", changes_array)

    return changes_struct

def create_status_effect_oneshot_and_tick(duration, interval, instant_adjustments, ticking_adjustments):
    """Create an FStatusEffectOneShotAndTick struct"""
    combined_struct = unreal.StatusEffectOneShotAndTick()
    combined_struct.set_editor_property("duration", duration)
    combined_struct.set_editor_property("interval", interval)

    instant_array = []
    for adj in instant_adjustments:
        stat_change = create_stat_change(adj["StatTag"], adj["MinAmount"], adj["MaxAmount"])
        instant_array.append(stat_change)
    combined_struct.set_editor_property("instant_stat_adjustment", instant_array)

    ticking_array = []
    for adj in ticking_adjustments:
        stat_change = create_stat_change(adj["StatTag"], adj["MinAmount"], adj["MaxAmount"])
        ticking_array.append(stat_change)
    combined_struct.set_editor_property("ticking_stat_adjustment", ticking_array)

    return combined_struct

def apply_status_effect_data(asset_name, data):
    """Apply RankInfo data to a status effect data asset"""
    path = f"/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/{asset_name}.{asset_name}"

    asset = unreal.EditorAssetLibrary.load_asset(path)
    if not asset:
        print(f"SKIP: Could not load {path}")
        return False

    print(f"\nProcessing: {asset_name}")

    # Apply TriggeredText
    if "TriggeredText" in data:
        try:
            asset.set_editor_property("triggered_text", data["TriggeredText"])
            print(f"  TriggeredText: {data['TriggeredText']}")
        except Exception as e:
            print(f"  WARNING: Could not set TriggeredText: {e}")

    # Get current RankInfo
    try:
        rank_info = asset.get_editor_property("rank_info")
        print(f"  Current RankInfo has {len(rank_info)} entries")
    except Exception as e:
        print(f"  WARNING: Could not get RankInfo: {e}")
        return False

    # Apply each rank's data
    for rank, rank_data in data.get("ranks", {}).items():
        rank_int = int(rank)
        print(f"  Processing Rank {rank_int}: {rank_data['type']}")

        # Get or create the rank entry
        if rank_int in rank_info:
            rank_entry = rank_info[rank_int]
        else:
            # Create new rank entry
            rank_entry = unreal.SLFStatusEffectRankInfo()
            rank_info[rank_int] = rank_entry

        # Create the appropriate struct based on type
        try:
            struct_type = rank_data["type"]

            if struct_type == "FStatusEffectTick":
                relevant_data = create_status_effect_tick(
                    rank_data["Duration"],
                    rank_data["Interval"],
                    rank_data["TickingStatAdjustment"]
                )
                # Set as FInstancedStruct
                instanced = unreal.InstancedStruct()
                instanced.set(relevant_data)
                rank_entry.set_editor_property("relevant_data", instanced)
                print(f"    Created FStatusEffectTick: Duration={rank_data['Duration']}, Interval={rank_data['Interval']}")

            elif struct_type == "FStatusEffectStatChanges":
                relevant_data = create_status_effect_stat_changes(rank_data["StatChanges"])
                instanced = unreal.InstancedStruct()
                instanced.set(relevant_data)
                rank_entry.set_editor_property("relevant_data", instanced)
                print(f"    Created FStatusEffectStatChanges: {len(rank_data['StatChanges'])} changes")

            elif struct_type == "FStatusEffectOneShotAndTick":
                relevant_data = create_status_effect_oneshot_and_tick(
                    rank_data["Duration"],
                    rank_data["Interval"],
                    rank_data.get("InstantStatAdjustment", []),
                    rank_data.get("TickingStatAdjustment", [])
                )
                instanced = unreal.InstancedStruct()
                instanced.set(relevant_data)
                rank_entry.set_editor_property("relevant_data", instanced)
                print(f"    Created FStatusEffectOneShotAndTick: Duration={rank_data['Duration']}")

            else:
                print(f"    WARNING: Unknown struct type: {struct_type}")
                continue

        except Exception as e:
            print(f"    ERROR creating struct: {e}")
            # Try simpler approach - just set the raw values on B_StatusEffect
            continue

    # Save the asset
    try:
        asset.set_editor_property("rank_info", rank_info)
        unreal.EditorAssetLibrary.save_asset(path)
        print(f"  SAVED: {asset_name}")
        return True
    except Exception as e:
        print(f"  ERROR saving: {e}")
        return False

def main():
    print("=" * 60)
    print("Status Effect RankInfo Restoration")
    print("=" * 60)

    success = 0
    failed = 0

    for asset_name, data in STATUS_EFFECT_DATA.items():
        try:
            if apply_status_effect_data(asset_name, data):
                success += 1
            else:
                failed += 1
        except Exception as e:
            print(f"ERROR processing {asset_name}: {e}")
            failed += 1

    print("\n" + "=" * 60)
    print(f"Results: {success} succeeded, {failed} failed")
    print("=" * 60)

if __name__ == "__main__":
    main()
