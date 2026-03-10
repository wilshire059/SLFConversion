import unreal

# Populate RelevantData on DA_Action_PickupItemMontage with an FMontage struct
# containing the pickup montage

da_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_PickupItemMontage"
montage_path = "/Game/SoulslikeFramework/Demo/_Animations/Interaction/ItemPickup/AM_SLF_ItemPickup"

da = unreal.EditorAssetLibrary.load_asset(da_path)
montage = unreal.EditorAssetLibrary.load_asset(montage_path)

print(f"DA: {da}")
print(f"Montage: {montage}")

if da and montage:
    # Get the RelevantData property
    try:
        relevant_data = da.get_editor_property('relevant_data')
        print(f"Current RelevantData: {relevant_data}")
        print(f"RelevantData type: {type(relevant_data)}")

        # Try to find and load the FMontage struct
        # FMontage is a Blueprint struct at /Game/SoulslikeFramework/Structures/Animation/FMontage
        fmontage_struct_path = "/Game/SoulslikeFramework/Structures/Animation/FMontage"
        fmontage_struct = unreal.EditorAssetLibrary.load_asset(fmontage_struct_path)
        print(f"FMontage struct: {fmontage_struct}")

        if fmontage_struct:
            # Create an instanced struct with FMontage type
            # This is complex in Python - we need to use make_instanced_struct
            try:
                # Try to create the struct instance with the montage
                # The FMontage struct has a property like "AnimMontage_3_B48455B6457133C3BC7F798BD0CB3493"
                # which is a soft object reference to AnimMontage

                # Since we can't easily create blueprint structs in Python,
                # let's try a different approach - set via property access

                # Check if there's a simpler way to set the struct data
                print("\nAttempting to set RelevantData...")

                # Create a new InstancedStruct - this may not be directly possible in Python
                # Alternative: Use BlueprintLibrary functions

                # Actually, let's check if we can access the FMontage property names
                if hasattr(fmontage_struct, 'get_class'):
                    struct_class = fmontage_struct.get_class()
                    print(f"FMontage class: {struct_class}")
                    print(f"FMontage class name: {struct_class.get_name() if struct_class else 'None'}")

            except Exception as e:
                print(f"Error creating struct: {e}")
                import traceback
                traceback.print_exc()

        # Alternative: Since setting RelevantData is complex, let's verify ActionMontage works
        # by checking if it's properly accessible
        print("\n=== Checking ActionMontage accessibility ===")
        try:
            action_montage = da.get_editor_property('action_montage')
            print(f"ActionMontage: {action_montage}")
            if action_montage:
                print(f"ActionMontage is set and accessible!")
                print("The issue is that the Blueprint code checks RelevantData, not ActionMontage")
                print("We need to either:")
                print("  1. Clear the Blueprint's EventGraph")
                print("  2. Populate RelevantData with FMontage struct")
        except Exception as e:
            print(f"Error getting ActionMontage: {e}")

    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()

else:
    print("Could not load required assets")

# Final recommendation
print("\n=== RECOMMENDATION ===")
print("The Blueprint B_Action_PickupItemMontage has EventGraph logic that overrides C++.")
print("The fix is to clear its EventGraph using the SLFAutomationLibrary::ClearEventGraph function")
print("Or manually in the editor: Open Blueprint > Delete Event ExecuteAction node > Compile > Save")
