# check_armor_data.py - Diagnose armor data asset state
import unreal

ARMOR_ASSETS = [
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleArmor02",
    "/Game/SoulslikeFramework/Data/Items/DA_ExampleHelmet",
]

def log(msg):
    print(msg)
    unreal.log_warning(msg)

log("=" * 60)
log("ARMOR DATA ASSET DIAGNOSTIC")
log("=" * 60)

for path in ARMOR_ASSETS:
    asset = unreal.EditorAssetLibrary.load_asset(path)
    if asset:
        log(f"\n[{path.split('/')[-1]}]")
        log(f"  Class: {asset.get_class().get_name()}")
        log(f"  Parent: {asset.get_class().get_super_class().get_name() if asset.get_class().get_super_class() else 'None'}")

        # Check ItemInformation
        try:
            item_info = asset.get_editor_property("item_information")
            log(f"  ItemInformation type: {type(item_info)}")

            # Try to get item name
            try:
                name = item_info.get_editor_property("item_name")
                log(f"  ItemName: '{name}'")
            except:
                log(f"  ItemName: <error reading>")

            # Try to get item class
            try:
                item_class = item_info.get_editor_property("item_class")
                log(f"  ItemClass: {item_class}")
            except:
                log(f"  ItemClass: <error reading>")

            # Try to get equipment details
            try:
                equip = item_info.get_editor_property("equipment_details")
                log(f"  EquipmentDetails type: {type(equip)}")

                # Check mesh info
                mesh_info = equip.get_editor_property("skeletal_mesh_info")
                log(f"  SkeletalMeshInfo count: {len(mesh_info) if mesh_info else 0}")
            except Exception as e:
                log(f"  EquipmentDetails: <error: {e}>")

        except Exception as e:
            log(f"  ItemInformation: <error: {e}>")
    else:
        log(f"\n[{path}] - FAILED TO LOAD")

log("\n" + "=" * 60)
log("DIAGNOSTIC COMPLETE")
log("=" * 60)
