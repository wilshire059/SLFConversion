# check_montage_notifies.py
# Check the AM_SLF_ItemPickup montage for notifies and duration

import unreal

def main():
    unreal.log_warning("=" * 70)
    unreal.log_warning("CHECKING AM_SLF_ItemPickup MONTAGE")
    unreal.log_warning("=" * 70)

    montage_path = "/Game/SoulslikeFramework/Demo/_Animations/Interaction/ItemPickup/AM_SLF_ItemPickup"
    montage = unreal.EditorAssetLibrary.load_asset(montage_path)

    if not montage:
        unreal.log_error(f"Failed to load: {montage_path}")
        return

    unreal.log_warning(f"Montage: {montage.get_name()}")

    # Get play length
    try:
        length = montage.get_editor_property("sequence_length")
        unreal.log_warning(f"Sequence Length: {length}")
    except:
        pass

    # Check for notifies
    unreal.log_warning("\n--- Animation Notifies ---")
    try:
        notifies = montage.get_editor_property("notifies")
        if notifies:
            for i, notify in enumerate(notifies):
                unreal.log_warning(f"  [{i}] {notify}")
        else:
            unreal.log_warning("  No notifies found")
    except Exception as e:
        unreal.log_warning(f"  Could not get notifies: {e}")

    # Export text for more details
    unreal.log_warning("\n--- Export Text (notify/time lines) ---")
    try:
        export_text = montage.export_text()
        for line in export_text.split('\n'):
            line_lower = line.lower()
            if 'notify' in line_lower or 'triggertime' in line_lower or 'linkvalue' in line_lower:
                unreal.log_warning(f"  {line.strip()}")
    except Exception as e:
        unreal.log_warning(f"  Could not export: {e}")

    unreal.log_warning("=" * 70)

if __name__ == "__main__":
    main()
