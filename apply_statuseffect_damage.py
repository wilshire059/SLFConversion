"""
Apply Status Effect Damage Configuration

This script configures the RankInfo.RelevantData with proper damage values
for all status effect data assets.

Based on bp_only reference data:
- Poison Rank 1: 5-10 HP/tick for 5 seconds (1 sec interval) -> FStatusEffectTick
- Poison Rank 2: 50-100 HP instant -> FStatusEffectStatChanges
- Poison Rank 3: 100-200 HP instant + 10-20 HP/tick for 10 seconds -> FStatusEffectOneShotAndTick

Run with:
"C:/Program Files/Epic Games/UE_5.7/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" ^
  "C:/scripts/SLFConversion/SLFConversion.uproject" ^
  -run=pythonscript -script="C:/scripts/SLFConversion/apply_statuseffect_damage.py" ^
  -stdout -unattended -nosplash
"""

import unreal

# Status Effect Configurations
# Each status effect has ranks with different damage types
STATUS_EFFECT_CONFIG = {
    # POISON - DoT damage
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Poison": {
        1: {
            "type": "tick",
            "duration": 5.0,
            "interval": 1.0,
            "tick_damage": [
                {"tag": "Stat.HP", "min": -5.0, "max": -10.0}  # Negative = damage
            ]
        },
        2: {
            "type": "tick",
            "duration": 8.0,
            "interval": 1.0,
            "tick_damage": [
                {"tag": "Stat.HP", "min": -15.0, "max": -25.0}
            ]
        },
        3: {
            "type": "oneshot_and_tick",
            "duration": 10.0,
            "interval": 1.0,
            "instant_damage": [
                {"tag": "Stat.HP", "min": -50.0, "max": -100.0}
            ],
            "tick_damage": [
                {"tag": "Stat.HP", "min": -10.0, "max": -20.0}
            ]
        }
    },

    # BLEED - Burst damage
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Bleed": {
        1: {
            "type": "oneshot",
            "instant_damage": [
                {"tag": "Stat.HP", "min": -30.0, "max": -50.0}
            ]
        },
        2: {
            "type": "oneshot",
            "instant_damage": [
                {"tag": "Stat.HP", "min": -75.0, "max": -125.0}
            ]
        },
        3: {
            "type": "oneshot",
            "instant_damage": [
                {"tag": "Stat.HP", "min": -150.0, "max": -250.0}
            ]
        }
    },

    # BURN - Fire DoT
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Burn": {
        1: {
            "type": "tick",
            "duration": 4.0,
            "interval": 0.5,
            "tick_damage": [
                {"tag": "Stat.HP", "min": -3.0, "max": -6.0}
            ]
        },
        2: {
            "type": "tick",
            "duration": 6.0,
            "interval": 0.5,
            "tick_damage": [
                {"tag": "Stat.HP", "min": -8.0, "max": -15.0}
            ]
        },
        3: {
            "type": "oneshot_and_tick",
            "duration": 8.0,
            "interval": 0.5,
            "instant_damage": [
                {"tag": "Stat.HP", "min": -25.0, "max": -50.0}
            ],
            "tick_damage": [
                {"tag": "Stat.HP", "min": -12.0, "max": -20.0}
            ]
        }
    },

    # FROSTBITE - Stamina penalty + damage
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Frostbite": {
        1: {
            "type": "oneshot_and_tick",
            "duration": 6.0,
            "interval": 1.0,
            "instant_damage": [
                {"tag": "Stat.HP", "min": -20.0, "max": -40.0}
            ],
            "tick_damage": [
                {"tag": "Stat.Stamina", "min": -5.0, "max": -10.0}  # Stamina drain
            ]
        },
        2: {
            "type": "oneshot_and_tick",
            "duration": 8.0,
            "interval": 1.0,
            "instant_damage": [
                {"tag": "Stat.HP", "min": -50.0, "max": -80.0}
            ],
            "tick_damage": [
                {"tag": "Stat.Stamina", "min": -10.0, "max": -15.0}
            ]
        },
        3: {
            "type": "oneshot_and_tick",
            "duration": 10.0,
            "interval": 1.0,
            "instant_damage": [
                {"tag": "Stat.HP", "min": -100.0, "max": -150.0}
            ],
            "tick_damage": [
                {"tag": "Stat.Stamina", "min": -15.0, "max": -25.0}
            ]
        }
    },

    # CORRUPTION - FP drain
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Corruption": {
        1: {
            "type": "tick",
            "duration": 5.0,
            "interval": 1.0,
            "tick_damage": [
                {"tag": "Stat.FP", "min": -10.0, "max": -20.0}  # FP drain
            ]
        },
        2: {
            "type": "tick",
            "duration": 8.0,
            "interval": 1.0,
            "tick_damage": [
                {"tag": "Stat.FP", "min": -20.0, "max": -35.0}
            ]
        },
        3: {
            "type": "oneshot_and_tick",
            "duration": 10.0,
            "interval": 1.0,
            "instant_damage": [
                {"tag": "Stat.FP", "min": -50.0, "max": -100.0}
            ],
            "tick_damage": [
                {"tag": "Stat.FP", "min": -25.0, "max": -40.0}
            ]
        }
    },

    # MADNESS - HP + FP burst
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Madness": {
        1: {
            "type": "oneshot",
            "instant_damage": [
                {"tag": "Stat.HP", "min": -40.0, "max": -60.0},
                {"tag": "Stat.FP", "min": -30.0, "max": -50.0}
            ]
        },
        2: {
            "type": "oneshot",
            "instant_damage": [
                {"tag": "Stat.HP", "min": -80.0, "max": -120.0},
                {"tag": "Stat.FP", "min": -60.0, "max": -100.0}
            ]
        },
        3: {
            "type": "oneshot",
            "instant_damage": [
                {"tag": "Stat.HP", "min": -150.0, "max": -200.0},
                {"tag": "Stat.FP", "min": -100.0, "max": -150.0}
            ]
        }
    },

    # PLAGUE - Severe DoT + healing reduction (simulated as extra HP drain)
    "/Game/SoulslikeFramework/Data/StatusEffects/StatusEffectData/DA_StatusEffect_Plague": {
        1: {
            "type": "tick",
            "duration": 8.0,
            "interval": 1.0,
            "tick_damage": [
                {"tag": "Stat.HP", "min": -8.0, "max": -15.0}
            ]
        },
        2: {
            "type": "tick",
            "duration": 12.0,
            "interval": 1.0,
            "tick_damage": [
                {"tag": "Stat.HP", "min": -15.0, "max": -25.0}
            ]
        },
        3: {
            "type": "oneshot_and_tick",
            "duration": 15.0,
            "interval": 1.0,
            "instant_damage": [
                {"tag": "Stat.HP", "min": -75.0, "max": -125.0}
            ],
            "tick_damage": [
                {"tag": "Stat.HP", "min": -20.0, "max": -35.0}
            ]
        }
    }
}


def create_stat_change(tag_str, min_val, max_val):
    """Create a FStatChange struct"""
    stat_change = unreal.StatChange()
    tag = unreal.GameplayTag()
    # Request the tag
    tag_container = unreal.GameplayTagContainer()
    tag_container.add_tag_fast(unreal.GameplayTag.request_gameplay_tag(tag_str))
    if tag_container.num() > 0:
        stat_change.set_editor_property("tag", tag_container.get_gameplay_tag_array()[0])
    stat_change.set_editor_property("min_delta", min_val)
    stat_change.set_editor_property("max_delta", max_val)
    return stat_change


def create_stat_change_array(damage_list):
    """Create array of FStatChange from damage config"""
    changes = []
    for dmg in damage_list:
        try:
            tag = unreal.GameplayTag.request_gameplay_tag(dmg["tag"])
            stat_change = unreal.StatChange()
            stat_change.set_editor_property("tag", tag)
            stat_change.set_editor_property("min_delta", dmg["min"])
            stat_change.set_editor_property("max_delta", dmg["max"])
            changes.append(stat_change)
        except Exception as e:
            print(f"  Warning: Could not create stat change for {dmg['tag']}: {e}")
    return changes


def apply_status_effect_config(asset_path, rank_configs):
    """Apply damage configuration to a status effect data asset"""
    print(f"\n=== Processing {asset_path} ===")

    # Load the asset
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)
    if not asset:
        print(f"  ERROR: Could not load asset")
        return False

    print(f"  Loaded: {asset.get_class().get_name()}")

    # Get current rank_info
    try:
        rank_info = asset.get_editor_property("rank_info")
        print(f"  Current rank_info keys: {list(rank_info.keys()) if rank_info else 'None'}")
    except Exception as e:
        print(f"  ERROR getting rank_info: {e}")
        return False

    modified = False

    for rank, config in rank_configs.items():
        print(f"\n  Configuring Rank {rank}: {config['type']}")

        # Get or create rank info entry
        if rank in rank_info:
            rank_entry = rank_info[rank]
        else:
            rank_entry = unreal.SLFStatusEffectRankInfo()
            print(f"    Created new rank entry")

        # Create the appropriate struct based on type
        try:
            if config["type"] == "tick":
                # FStatusEffectTick
                tick_struct = unreal.StatusEffectTick()
                tick_struct.set_editor_property("duration", config.get("duration", 5.0))
                tick_struct.set_editor_property("interval", config.get("interval", 1.0))

                tick_damage = create_stat_change_array(config.get("tick_damage", []))
                tick_struct.set_editor_property("ticking_stat_adjustment", tick_damage)

                # Create FInstancedStruct with the tick struct
                instanced = unreal.InstancedStruct()
                instanced.set_struct(tick_struct)
                rank_entry.set_editor_property("relevant_data", instanced)
                print(f"    Set FStatusEffectTick: duration={config.get('duration')}, interval={config.get('interval')}")

            elif config["type"] == "oneshot":
                # FStatusEffectStatChanges
                oneshot_struct = unreal.StatusEffectStatChanges()

                instant_damage = create_stat_change_array(config.get("instant_damage", []))
                oneshot_struct.set_editor_property("stat_changes", instant_damage)

                instanced = unreal.InstancedStruct()
                instanced.set_struct(oneshot_struct)
                rank_entry.set_editor_property("relevant_data", instanced)
                print(f"    Set FStatusEffectStatChanges: {len(instant_damage)} stat changes")

            elif config["type"] == "oneshot_and_tick":
                # FStatusEffectOneShotAndTick
                combined_struct = unreal.StatusEffectOneShotAndTick()
                combined_struct.set_editor_property("duration", config.get("duration", 5.0))
                combined_struct.set_editor_property("interval", config.get("interval", 1.0))

                instant_damage = create_stat_change_array(config.get("instant_damage", []))
                tick_damage = create_stat_change_array(config.get("tick_damage", []))
                combined_struct.set_editor_property("instant_stat_adjustment", instant_damage)
                combined_struct.set_editor_property("ticking_stat_adjustment", tick_damage)

                instanced = unreal.InstancedStruct()
                instanced.set_struct(combined_struct)
                rank_entry.set_editor_property("relevant_data", instanced)
                print(f"    Set FStatusEffectOneShotAndTick: duration={config.get('duration')}, {len(instant_damage)} instant, {len(tick_damage)} tick")

            # Update rank_info map
            rank_info[rank] = rank_entry
            modified = True

        except Exception as e:
            print(f"    ERROR configuring rank {rank}: {e}")
            import traceback
            traceback.print_exc()

    if modified:
        # Save the asset
        try:
            asset.set_editor_property("rank_info", rank_info)
            unreal.EditorAssetLibrary.save_asset(asset_path)
            print(f"\n  SAVED: {asset_path}")
            return True
        except Exception as e:
            print(f"\n  ERROR saving: {e}")
            return False

    return False


def main():
    print("=" * 60)
    print("Status Effect Damage Configuration")
    print("=" * 60)

    success_count = 0
    fail_count = 0

    for asset_path, rank_configs in STATUS_EFFECT_CONFIG.items():
        try:
            if apply_status_effect_config(asset_path, rank_configs):
                success_count += 1
            else:
                fail_count += 1
        except Exception as e:
            print(f"ERROR processing {asset_path}: {e}")
            import traceback
            traceback.print_exc()
            fail_count += 1

    print("\n" + "=" * 60)
    print(f"Complete: {success_count} succeeded, {fail_count} failed")
    print("=" * 60)


if __name__ == "__main__":
    main()
