"""
Extract weapon stat data from bp_only project
Uses get_editor_property to access the Blueprint properties
"""
import unreal
import json
import os

def extract_weapon_stats():
    """Extract weapon stat info from all weapon data assets"""

    # Weapon data assets in bp_only
    weapon_paths = [
        "/Game/SoulslikeFramework/Data/Items/DA_Katana",
        "/Game/SoulslikeFramework/Data/Items/DA_Sword01",
        "/Game/SoulslikeFramework/Data/Items/DA_Greatsword",
        "/Game/SoulslikeFramework/Data/Items/DA_Shield",
        "/Game/SoulslikeFramework/Data/Items/DA_PoisonSword",
        "/Game/SoulslikeFramework/Data/Items/DA_BossMace",
    ]

    weapon_data = {}

    for asset_path in weapon_paths:
        asset = unreal.load_asset(asset_path)
        if not asset:
            print(f"ERROR: Could not load {asset_path}")
            continue

        asset_name = asset.get_name()
        print(f"\n=== {asset_name} ===")
        print(f"Class: {asset.get_class().get_name()}")

        weapon_info = {}

        # Try to get ItemInformation struct
        try:
            item_info = asset.get_editor_property('item_information')
            if item_info:
                print("  Got item_information (snake_case)")
                weapon_info['has_item_info'] = True

                # Try to get EquipmentDetails
                try:
                    equip_details = item_info.get_editor_property('equipment_details')
                    if equip_details:
                        print("  Got equipment_details")

                        # Try to get WeaponStatInfo
                        try:
                            weapon_stat_info = equip_details.get_editor_property('weapon_stat_info')
                            if weapon_stat_info:
                                print("  Got weapon_stat_info")

                                # Extract bHasStatScaling
                                try:
                                    has_scaling = weapon_stat_info.get_editor_property('has_stat_scaling')
                                    weapon_info['has_stat_scaling'] = has_scaling
                                    print(f"    has_stat_scaling: {has_scaling}")
                                except:
                                    pass

                                # Extract bHasStatRequirement
                                try:
                                    has_req = weapon_stat_info.get_editor_property('has_stat_requirement')
                                    weapon_info['has_stat_requirement'] = has_req
                                    print(f"    has_stat_requirement: {has_req}")
                                except:
                                    pass

                                # Extract ScalingInfo
                                try:
                                    scaling_info = weapon_stat_info.get_editor_property('scaling_info')
                                    if scaling_info:
                                        scaling_dict = {}
                                        for key in scaling_info.keys():
                                            scaling_dict[str(key)] = str(scaling_info[key])
                                        weapon_info['scaling_info'] = scaling_dict
                                        print(f"    scaling_info: {scaling_dict}")
                                except Exception as e:
                                    print(f"    scaling_info error: {e}")

                                # Extract StatRequirementInfo
                                try:
                                    req_info = weapon_stat_info.get_editor_property('stat_requirement_info')
                                    if req_info:
                                        req_dict = {}
                                        for key in req_info.keys():
                                            req_dict[str(key)] = int(req_info[key])
                                        weapon_info['stat_requirement_info'] = req_dict
                                        print(f"    stat_requirement_info: {req_dict}")
                                except Exception as e:
                                    print(f"    stat_requirement_info error: {e}")
                        except Exception as e:
                            print(f"  weapon_stat_info error: {e}")

                        # Try to get StatChanges
                        try:
                            stat_changes = equip_details.get_editor_property('stat_changes')
                            if stat_changes:
                                changes_dict = {}
                                for key in stat_changes.keys():
                                    changes_dict[str(key)] = float(stat_changes[key])
                                weapon_info['stat_changes'] = changes_dict
                                print(f"    stat_changes: {changes_dict}")
                        except Exception as e:
                            print(f"  stat_changes error: {e}")

                except Exception as e:
                    print(f"  equipment_details error: {e}")
        except:
            pass

        # Also try PascalCase property names (Blueprint style)
        try:
            item_info = asset.get_editor_property('ItemInformation')
            if item_info:
                print("  Got ItemInformation (PascalCase)")
                weapon_info['has_item_info_pascal'] = True

                try:
                    equip_details = item_info.get_editor_property('EquipmentDetails')
                    if equip_details:
                        print("  Got EquipmentDetails (PascalCase)")

                        try:
                            weapon_stat_info = equip_details.get_editor_property('WeaponStatInfo')
                            if weapon_stat_info:
                                print("  Got WeaponStatInfo (PascalCase)")

                                try:
                                    has_scaling = weapon_stat_info.get_editor_property('bHasStatScaling')
                                    weapon_info['has_stat_scaling'] = has_scaling
                                    print(f"    bHasStatScaling: {has_scaling}")
                                except Exception as e:
                                    print(f"    bHasStatScaling error: {e}")

                                try:
                                    has_req = weapon_stat_info.get_editor_property('bHasStatRequirement')
                                    weapon_info['has_stat_requirement'] = has_req
                                    print(f"    bHasStatRequirement: {has_req}")
                                except Exception as e:
                                    print(f"    bHasStatRequirement error: {e}")

                                try:
                                    scaling_info = weapon_stat_info.get_editor_property('ScalingInfo')
                                    if scaling_info:
                                        scaling_dict = {}
                                        for key in scaling_info.keys():
                                            scaling_dict[str(key)] = str(scaling_info[key])
                                        weapon_info['scaling_info'] = scaling_dict
                                        print(f"    ScalingInfo: {scaling_dict}")
                                except Exception as e:
                                    print(f"    ScalingInfo error: {e}")

                                try:
                                    req_info = weapon_stat_info.get_editor_property('StatRequirementInfo')
                                    if req_info:
                                        req_dict = {}
                                        for key in req_info.keys():
                                            req_dict[str(key)] = int(req_info[key])
                                        weapon_info['stat_requirement_info'] = req_dict
                                        print(f"    StatRequirementInfo: {req_dict}")
                                except Exception as e:
                                    print(f"    StatRequirementInfo error: {e}")
                        except Exception as e:
                            print(f"  WeaponStatInfo error: {e}")

                        try:
                            stat_changes = equip_details.get_editor_property('StatChanges')
                            if stat_changes:
                                changes_dict = {}
                                for key in stat_changes.keys():
                                    changes_dict[str(key)] = float(stat_changes[key])
                                weapon_info['stat_changes'] = changes_dict
                                print(f"    StatChanges: {changes_dict}")
                        except Exception as e:
                            print(f"  StatChanges error: {e}")

                except Exception as e:
                    print(f"  EquipmentDetails error: {e}")
        except:
            pass

        # List all available properties
        print("\n  Available properties:")
        for prop in dir(asset):
            if not prop.startswith('_'):
                print(f"    {prop}")

        weapon_data[asset_name] = weapon_info

    # Save to cache
    output_path = "C:/scripts/SLFConversion/migration_cache/weapon_stats.json"
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    with open(output_path, 'w', encoding='utf-8') as f:
        json.dump(weapon_data, f, indent=2)
    print(f"\nSaved to: {output_path}")

if __name__ == "__main__":
    extract_weapon_stats()
