"""
Reparent vendor widgets to C++ classes.
This fixes the 'No SignatureFunction in MulticastDelegateProperty' errors
by ensuring the delegate declarations come from C++.
"""
import unreal

# Vendor widgets and their C++ parent classes
VENDOR_WIDGETS = {
    "/Game/SoulslikeFramework/Widgets/Vendor/W_VendorSlot": "/Script/SLFConversion.W_VendorSlot",
    "/Game/SoulslikeFramework/Widgets/Vendor/W_VendorAction": "/Script/SLFConversion.W_VendorAction",
    "/Game/SoulslikeFramework/Widgets/Vendor/W_NPC_Window_Vendor": "/Script/SLFConversion.W_NPC_Window_Vendor",
}

def reparent_widget(bp_path, cpp_class_path):
    """Reparent a single widget Blueprint to its C++ class."""
    print(f"\n=== Processing {bp_path} ===")

    # Load the Blueprint
    bp_asset = unreal.load_asset(bp_path)
    if not bp_asset:
        print(f"  ERROR: Could not load Blueprint")
        return False

    print(f"  Blueprint loaded: {bp_asset.get_name()}")

    # Check current parent
    gen_class = bp_asset.generated_class()
    if gen_class:
        current_parent = gen_class.get_super_class()
        if current_parent:
            print(f"  Current parent: {current_parent.get_name()}")

            # Check if already reparented to target
            if cpp_class_path.split('.')[-1] in current_parent.get_path_name():
                print(f"  Already reparented to C++ class - skipping")
                return True

    # Load the target C++ class
    cpp_class = unreal.load_class(None, cpp_class_path)
    if not cpp_class:
        print(f"  ERROR: Could not load C++ class: {cpp_class_path}")
        return False

    print(f"  Target C++ class: {cpp_class.get_name()}")

    # Reparent
    try:
        result = unreal.BlueprintEditorLibrary.reparent_blueprint(bp_asset, cpp_class)
        if result:
            print(f"  SUCCESS: Reparented to {cpp_class.get_name()}")

            # Compile the Blueprint
            unreal.BlueprintEditorLibrary.compile_blueprint(bp_asset)
            print(f"  Compiled Blueprint")

            # Save
            unreal.EditorAssetLibrary.save_asset(bp_path)
            print(f"  Saved")
            return True
        else:
            print(f"  FAILED: Reparent returned false")
            return False
    except Exception as e:
        print(f"  ERROR: {e}")
        return False

def main():
    print("=" * 60)
    print("VENDOR WIDGET REPARENTING TO C++")
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
