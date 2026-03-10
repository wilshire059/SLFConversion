"""
Verify PDA_BaseCharacterInfo reparenting worked.
"""
import unreal

# Check DA_Quinn and DA_Manny
for da_name in ["DA_Quinn", "DA_Manny"]:
    da_path = f"/Game/SoulslikeFramework/Data/BaseCharacters/{da_name}"
    da_asset = unreal.EditorAssetLibrary.load_asset(da_path)
    if da_asset:
        cls = da_asset.get_class()
        unreal.log_warning(f"VERIFY {da_name}: class={cls.get_name()}")

        try:
            cn = da_asset.get_editor_property("character_class_name")
            unreal.log_warning(f"VERIFY {da_name}: CharacterClassName='{cn}'")
        except Exception as e:
            unreal.log_warning(f"VERIFY {da_name}: CharacterClassName FAIL: {e}")

        try:
            dn = da_asset.get_editor_property("display_name")
            unreal.log_warning(f"VERIFY {da_name}: DisplayName='{dn}'")
        except Exception as e:
            unreal.log_warning(f"VERIFY {da_name}: DisplayName FAIL: {e}")

        try:
            bs = da_asset.get_editor_property("base_stats")
            unreal.log_warning(f"VERIFY {da_name}: BaseStats={len(bs)} entries")
        except Exception as e:
            unreal.log_warning(f"VERIFY {da_name}: BaseStats FAIL: {e}")
    else:
        unreal.log_warning(f"VERIFY {da_name}: COULD NOT LOAD")

unreal.log_warning("VERIFY COMPLETE")
