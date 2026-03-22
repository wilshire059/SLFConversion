"""
Surgical reparent script: PDA_BaseCharacterInfo -> UPDA_BaseCharacterInfo
Note: reparent_blueprint returns None (void), don't check return value.
"""
import unreal

BP_PATH = "/Game/SoulslikeFramework/Data/PDA_BaseCharacterInfo"
NEW_PARENT = "/Script/SLFConversion.PDA_BaseCharacterInfo"

def reparent():
    asset = unreal.EditorAssetLibrary.load_asset(BP_PATH)
    if not asset:
        unreal.log_warning(f"ERROR: Could not load {BP_PATH}")
        return

    bp = unreal.Blueprint.cast(asset)
    if not bp:
        unreal.log_warning(f"ERROR: Not a Blueprint: {type(asset)}")
        return

    new_parent_class = unreal.load_class(None, NEW_PARENT)
    if not new_parent_class:
        unreal.log_warning(f"ERROR: Could not load parent: {NEW_PARENT}")
        return

    unreal.log_warning(f"Reparenting {BP_PATH} to {NEW_PARENT}")

    # Reparent (void return)
    unreal.BlueprintEditorLibrary.reparent_blueprint(bp, new_parent_class)
    unreal.log_warning("Reparent called")

    # Compile
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    unreal.log_warning("Compiled")

    # Save the Blueprint
    unreal.EditorAssetLibrary.save_loaded_asset(asset, False)
    unreal.EditorAssetLibrary.save_asset(BP_PATH, only_if_is_dirty=False)
    unreal.log_warning("Saved PDA_BaseCharacterInfo")

    # Resave data assets to pick up new parent
    for da_name in ["DA_Quinn", "DA_Manny"]:
        da_path = f"/Game/SoulslikeFramework/Data/BaseCharacters/{da_name}"
        da_asset = unreal.EditorAssetLibrary.load_asset(da_path)
        if da_asset:
            unreal.EditorAssetLibrary.save_loaded_asset(da_asset, False)
            unreal.log_warning(f"Resaved {da_name}")

    # Verify
    for da_name in ["DA_Quinn", "DA_Manny"]:
        da_path = f"/Game/SoulslikeFramework/Data/BaseCharacters/{da_name}"
        da_asset = unreal.EditorAssetLibrary.load_asset(da_path)
        if da_asset:
            cls = da_asset.get_class()
            unreal.log_warning(f"FINAL {da_name}: class={cls.get_name()}")
            try:
                cn = da_asset.get_editor_property("character_class_name")
                unreal.log_warning(f"FINAL {da_name}: CharacterClassName='{cn}'")
            except:
                unreal.log_warning(f"FINAL {da_name}: CharacterClassName NOT ACCESSIBLE")
        else:
            unreal.log_warning(f"FINAL {da_name}: COULD NOT LOAD")

reparent()
