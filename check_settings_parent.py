import unreal
import re

paths = [
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings",
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry", 
    "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_CategoryEntry",
    "/Game/SoulslikeFramework/Data/PDA_CustomSettings",
]

for bp_path in paths:
    name = bp_path.split('/')[-1]
    try:
        bp = unreal.load_asset(bp_path)
        if bp is None:
            unreal.log_warning(f"CHECK: {name}: NOT FOUND")
            continue
        
        # Get the Exporter and export to string
        export = unreal.Exporter.export_to_string(bp)
        
        # Look for ParentClass in the export
        if export:
            parent_match = re.search(r'ParentClass=Class\'([^\']+)\'', export)
            if parent_match:
                unreal.log_warning(f"CHECK: {name}: Parent = {parent_match.group(1)}")
            else:
                # Try another pattern
                parent_match2 = re.search(r'ParentClass=[^\s,]+', export)
                if parent_match2:
                    unreal.log_warning(f"CHECK: {name}: {parent_match2.group(0)}")
                else:
                    unreal.log_warning(f"CHECK: {name}: No ParentClass pattern found")
        else:
            unreal.log_warning(f"CHECK: {name}: Empty export text")
    except Exception as e:
        unreal.log_warning(f"CHECK: {name}: Error - {e}")

unreal.log_warning("CHECK: Done")
