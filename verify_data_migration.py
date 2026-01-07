import unreal

unreal.log_warning("=" * 60)
unreal.log_warning("DATA MIGRATION VERIFICATION")
unreal.log_warning("=" * 60)

# Check Dodge Montages
unreal.log_warning("")
unreal.log_warning("=== DODGE MONTAGES ===")
try:
    dodge_asset = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_Dodge")
    if dodge_asset:
        dodge_montages = dodge_asset.get_editor_property('DodgeMontages')
        unreal.log_warning("Forward: " + str(dodge_montages.forward))
        unreal.log_warning("Backward: " + str(dodge_montages.backward))
        unreal.log_warning("Left: " + str(dodge_montages.left))
        unreal.log_warning("Backstep: " + str(dodge_montages.backstep))
    else:
        unreal.log_warning("ERROR: Could not load DA_Action_Dodge")
except Exception as e:
    unreal.log_warning("ERROR: " + str(e))

# Check Item Niagara Systems (just check a few)
unreal.log_warning("")
unreal.log_warning("=== ITEM NIAGARA SYSTEMS ===")
items_to_check = ["DA_Apple", "DA_Greatsword", "DA_Katana"]
for item_name in items_to_check:
    try:
        item_asset = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Data/Items/" + item_name)
        if item_asset:
            niagara = item_asset.get_editor_property('world_niagara_system')
            unreal.log_warning(item_name + ": " + (str(niagara) if niagara else "NULL"))
        else:
            unreal.log_warning(item_name + ": ASSET NOT FOUND")
    except Exception as e:
        unreal.log_warning(item_name + ": ERROR - " + str(e))

# Check DefaultMeshAsset
unreal.log_warning("")
unreal.log_warning("=== CHARACTER DEFAULT MESH ASSETS ===")
chars_to_check = ["DA_Manny", "DA_Quinn"]
for char_name in chars_to_check:
    try:
        char_asset = unreal.EditorAssetLibrary.load_asset("/Game/SoulslikeFramework/Data/BaseCharacters/" + char_name)
        if char_asset:
            mesh = char_asset.get_editor_property('default_mesh_asset')
            unreal.log_warning(char_name + ": " + (mesh.get_name() if mesh else "NULL"))
        else:
            unreal.log_warning(char_name + ": ASSET NOT FOUND")
    except Exception as e:
        unreal.log_warning(char_name + ": ERROR - " + str(e))

unreal.log_warning("")
unreal.log_warning("=== VERIFICATION COMPLETE ===")
