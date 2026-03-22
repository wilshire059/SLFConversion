"""
Reparent W_NPC_Window_Vendor Blueprint to C++ class UW_NPC_Window_Vendor
This allows C++ function calls to work directly.
"""
import unreal

def reparent_vendor_widget():
    # Load the Blueprint
    bp_path = "/Game/SoulslikeFramework/Widgets/Vendor/W_NPC_Window_Vendor"
    bp_asset = unreal.load_asset(bp_path)

    if not bp_asset:
        print(f"ERROR: Could not load Blueprint at {bp_path}")
        return False

    print(f"Loaded Blueprint: {bp_asset.get_name()}")

    # Get the C++ parent class
    cpp_class_path = "/Script/SLFConversion.W_NPC_Window_Vendor"
    cpp_class = unreal.load_class(None, cpp_class_path)

    if not cpp_class:
        print(f"ERROR: Could not load C++ class at {cpp_class_path}")
        return False

    print(f"Target C++ class: {cpp_class.get_name()}")

    # Reparent using BlueprintEditorLibrary
    result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp_asset, cpp_class)

    if result:
        print(f"SUCCESS: Reparented {bp_asset.get_name()} to {cpp_class.get_name()}")

        # Save the asset
        unreal.EditorAssetLibrary.save_asset(bp_path)
        print(f"Saved: {bp_path}")
        return True
    else:
        print(f"FAILED: Could not reparent Blueprint")
        return False

# Run
reparent_vendor_widget()
