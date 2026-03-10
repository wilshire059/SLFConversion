"""
Reparent ALL vendor widgets to their C++ classes.
This fixes the "No SignatureFunction in MulticastDelegateProperty" errors.
"""
import unreal

VENDOR_WIDGETS = {
    "/Game/SoulslikeFramework/Widgets/Vendor/W_VendorSlot": "/Script/SLFConversion.W_VendorSlot",
    "/Game/SoulslikeFramework/Widgets/Vendor/W_VendorAction": "/Script/SLFConversion.W_VendorAction",
    "/Game/SoulslikeFramework/Widgets/Vendor/W_NPC_Window_Vendor": "/Script/SLFConversion.W_NPC_Window_Vendor",
}

def reparent_widget(bp_path, cpp_class_path):
    """Reparent a single widget Blueprint to its C++ class."""
    print(f"\n=== Reparenting {bp_path} ===")

    # Load the Blueprint
    bp_asset = unreal.load_asset(bp_path)
    if not bp_asset:
        print(f"  ERROR: Could not load Blueprint at {bp_path}")
        return False

    print(f"  Loaded Blueprint: {bp_asset.get_name()}")

    # Get the C++ parent class
    cpp_class = unreal.load_class(None, cpp_class_path)
    if not cpp_class:
        print(f"  ERROR: Could not load C++ class at {cpp_class_path}")
        return False

    print(f"  Target C++ class: {cpp_class.get_name()}")

    # Reparent using BlueprintEditorLibrary
    result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp_asset, cpp_class)

    if result:
        print(f"  SUCCESS: Reparented {bp_asset.get_name()} to {cpp_class.get_name()}")

        # Save the asset
        unreal.EditorAssetLibrary.save_asset(bp_path)
        print(f"  Saved: {bp_path}")
        return True
    else:
        print(f"  FAILED: Could not reparent Blueprint")
        return False

def main():
    print("=" * 60)
    print("VENDOR WIDGET REPARENTING")
    print("=" * 60)

    success_count = 0
    fail_count = 0

    for bp_path, cpp_path in VENDOR_WIDGETS.items():
        if reparent_widget(bp_path, cpp_path):
            success_count += 1
        else:
            fail_count += 1

    print("\n" + "=" * 60)
    print(f"RESULTS: {success_count} succeeded, {fail_count} failed")
    print("=" * 60)

# Run
main()
