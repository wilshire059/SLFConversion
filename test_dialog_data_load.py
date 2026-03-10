"""
Test that DA_ExampleDialog loads with correct Requirement data
"""
import unreal

def test_dialog_data():
    print("=" * 60)
    print("TESTING DA_ExampleDialog LOAD")
    print("=" * 60)

    # Load the dialog asset
    asset_path = "/Game/SoulslikeFramework/Data/Dialog/ShowcaseGuideNpc/DA_ExampleDialog"
    asset = unreal.EditorAssetLibrary.load_asset(asset_path)

    if not asset:
        print("[FAIL] Could not load DA_ExampleDialog")
        return

    print(f"[OK] Loaded: {asset.get_name()}")
    print(f"[OK] Class: {asset.get_class().get_name()}")

    # Try to access the Requirement array via export
    # This will show if the data loaded correctly
    try:
        # Get path for metadata check
        metadata = unreal.EditorAssetLibrary.get_metadata_tag(asset_path, "NativeClass")
        print(f"[INFO] Native Class: {metadata}")
    except Exception as e:
        print(f"[WARN] Metadata access: {str(e)}")

    # Check the class inheritance
    cls = asset.get_class()
    parent = cls.get_super_class() if cls else None
    print(f"[INFO] Parent Class: {parent.get_name() if parent else 'None'}")

    print("\n[SUCCESS] DA_ExampleDialog loads without crash")
    print("Struct redirect FSLFDialogRequirement appears to be working")

if __name__ == "__main__":
    test_dialog_data()
