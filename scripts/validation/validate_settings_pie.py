"""
PIE test to validate settings widgets show correct values.
"""
import unreal

def main():
    unreal.log_warning("=" * 60)
    unreal.log_warning("VALIDATING SETTINGS IN PIE")
    unreal.log_warning("=" * 60)
    
    # Check W_Settings_Entry widget class for OnGenerateContentWidget binding
    entry_bp_path = "/Game/SoulslikeFramework/Widgets/SettingsMenu/W_Settings_Entry"
    entry_bp = unreal.load_asset(entry_bp_path)
    
    if entry_bp:
        unreal.log_warning(f"W_Settings_Entry loaded: {entry_bp.get_name()}")
        unreal.log_warning(f"  Parent class: {entry_bp.parent_class.get_name() if entry_bp.parent_class else 'None'}")
        
        # Check for delegate bindings
        result = unreal.SLFAutomationLibrary.diagnose_settings_widgets()
        unreal.log_warning(result)
    else:
        unreal.log_warning("ERROR: Failed to load W_Settings_Entry")
    
    unreal.log_warning("=" * 60)

main()
