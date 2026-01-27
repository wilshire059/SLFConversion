# check_widget_parents.py
# Check the parent classes of key widgets to verify reparenting

import unreal

def check_widget_parents():
    """Check widget Blueprint parent classes using export_text."""

    widgets_to_check = [
        "/Game/SoulslikeFramework/Widgets/Status/W_Status",
        "/Game/SoulslikeFramework/Widgets/Status/W_Status_StatBlock",
        "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry_Status",
        "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry_StatName",
        "/Game/SoulslikeFramework/Widgets/Stats/W_StatEntry",
        "/Game/SoulslikeFramework/Widgets/Stats/W_StatBlock",
    ]

    unreal.log_warning("=" * 60)
    unreal.log_warning("Checking widget Blueprint parent classes")
    unreal.log_warning("=" * 60)

    for widget_path in widgets_to_check:
        bp_name = widget_path.split("/")[-1]

        # Use export_text to get Blueprint text representation
        try:
            text = unreal.EditorAssetLibrary.export_text(widget_path)
            if not text:
                unreal.log_warning(f"{bp_name}: Could not export text")
                continue

            # Find ParentClass line in the text
            # Look for pattern like: ParentClass=Class'/Script/UMG.UserWidget'
            # or ParentClass=BlueprintGeneratedClass'/Game/...'
            lines = text.split('\n')
            for line in lines[:100]:  # Only check first 100 lines
                if 'ParentClass=' in line:
                    # Extract just the class name
                    unreal.log_warning(f"\n{bp_name}")
                    unreal.log_warning(f"  {line.strip()}")
                    break
        except Exception as e:
            unreal.log_warning(f"{bp_name}: Error - {e}")

    unreal.log_warning("\n" + "=" * 60)

if __name__ == "__main__":
    check_widget_parents()
