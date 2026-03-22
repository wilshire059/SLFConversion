"""
Diagnose current state of Settings widgets after backup restore.
"""
import unreal

unreal.log_warning("=== DIAGNOSE SETTINGS WIDGETS STATE ===\n")

# Run the C++ diagnose function
result = unreal.SLFAutomationLibrary.diagnose_settings_widgets()
unreal.log_warning(result)
