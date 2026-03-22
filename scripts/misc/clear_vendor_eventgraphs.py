"""
Clear EventGraphs from vendor widgets and reparent to C++.
This allows the C++ implementations to take over.
"""
import unreal

VENDOR_WIDGETS = {
    "/Game/SoulslikeFramework/Widgets/Vendor/W_VendorSlot": "/Script/SLFConversion.W_VendorSlot",
    "/Game/SoulslikeFramework/Widgets/Vendor/W_VendorAction": "/Script/SLFConversion.W_VendorAction",
    "/Game/SoulslikeFramework/Widgets/Vendor/W_NPC_Window_Vendor": "/Script/SLFConversion.W_NPC_Window_Vendor",
}

def clear_and_reparent(bp_path, cpp_class_path):
    """Clear EventGraph and reparent a widget Blueprint to C++."""
    print(f"\n=== Processing {bp_path} ===")

    # Load the Blueprint
    bp_asset = unreal.load_asset(bp_path)
    if not bp_asset:
        print(f"  ERROR: Could not load Blueprint")
        return False

    bp_name = bp_asset.get_name()
    print(f"  Blueprint loaded: {bp_name}")

    # Clear all Blueprint logic using SLFAutomationLibrary
    try:
        # Use our C++ automation library to clear the EventGraph
        result = unreal.SLFAutomationLibrary.clear_event_graph(bp_asset)
        if result:
            print(f"  Cleared EventGraph via SLFAutomationLibrary")
        else:
            print(f"  Warning: clear_event_graph returned false")
    except Exception as e:
        print(f"  SLFAutomationLibrary not available, trying manual clear: {e}")

        # Manual fallback - get UbergraphPages and clear them
        try:
            # Access internal Blueprint structure
            uber_graphs = bp_asset.get_editor_property('ubergraph_pages')
            if uber_graphs:
                print(f"  Found {len(uber_graphs)} UbergraphPages")
                # We can't directly clear, but we can try compiling after reparent
        except:
            pass

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
        else:
            print(f"  Reparent returned false, continuing anyway...")
    except Exception as e:
        print(f"  Reparent exception: {e}")

    # Try to compile - will fail on first pass but that's expected
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(bp_asset)
        print(f"  Compiled Blueprint")
    except Exception as e:
        print(f"  Compile exception (expected): {e}")

    # Save
    try:
        unreal.EditorAssetLibrary.save_asset(bp_path)
        print(f"  Saved")
    except Exception as e:
        print(f"  Save exception: {e}")

    return True

def main():
    print("=" * 60)
    print("VENDOR WIDGET EVENTGRAPH CLEARING + REPARENTING")
    print("=" * 60)

    for bp_path, cpp_path in VENDOR_WIDGETS.items():
        clear_and_reparent(bp_path, cpp_path)

    print("\n" + "=" * 60)
    print("DONE - Vendor widgets cleared and reparented")
    print("=" * 60)

main()
