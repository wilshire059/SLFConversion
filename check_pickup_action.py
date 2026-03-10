import unreal

output_file = "C:/scripts/SLFConversion/pickup_action_check.txt"
lines = []

def log(msg):
    print(msg)
    lines.append(str(msg))

# Check the pickup action data asset
da_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_PickupItemMontage"
da = unreal.EditorAssetLibrary.load_asset(da_path)

if da:
    log(f"Found: {da.get_name()}")
    log(f"Class: {da.get_class().get_name()}")

    # Try to get RelevantData specifically
    try:
        relevant_data = da.get_editor_property('relevant_data')
        log(f"\nRelevantData: {relevant_data}")
        log(f"RelevantData type: {type(relevant_data)}")

        # If it's an InstancedStruct, try to get more info
        if relevant_data:
            try:
                log(f"RelevantData is_valid: {relevant_data.is_valid()}")
                log(f"RelevantData script_struct: {relevant_data.get_script_struct()}")
            except Exception as e:
                log(f"Error inspecting RelevantData: {e}")
    except Exception as e:
        log(f"Error getting RelevantData: {e}")

    # Try ActionMontage
    try:
        action_montage = da.get_editor_property('action_montage')
        log(f"\nActionMontage: {action_montage}")
    except Exception as e:
        log(f"Error getting ActionMontage: {e}")

    # Try ActionClass
    try:
        action_class = da.get_editor_property('action_class')
        log(f"\nActionClass: {action_class}")
    except Exception as e:
        log(f"Error getting ActionClass: {e}")
else:
    log(f"Could not load: {da_path}")

# Check general montages folder for pickup-related montages
log("\n\n=== Searching for pickup montages ===")
montage_path = "/Game/SoulslikeFramework/Demo/_Animations/Montages"
try:
    all_montages = unreal.EditorAssetLibrary.list_assets(montage_path, recursive=True)
    pickup_montages = [m for m in all_montages if 'pickup' in m.lower() or 'loot' in m.lower()]
    log(f"Pickup/Loot related montages found: {len(pickup_montages)}")
    for m in pickup_montages:
        log(f"  {m}")

    if len(pickup_montages) == 0:
        log("No pickup montages found. Listing all montages:")
        for m in all_montages[:20]:
            log(f"  {m}")
except Exception as e:
    log(f"Error listing montages: {e}")

# Check bp_only backup for comparison
log("\n\n=== Checking bp_only backup ===")
try:
    bp_only_path = "/Game/SoulslikeFramework/Data/Actions/ActionData/DA_Action_PickupItemMontage"
    # Can't directly load from bp_only, but we can list what's there
    log("(Note: Cannot load from backup project directly)")
except Exception as e:
    log(f"Error: {e}")

# Save output
with open(output_file, 'w') as f:
    f.write('\n'.join(lines))

log(f"\n\nOutput saved to: {output_file}")
