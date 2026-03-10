import unreal
result = unreal.SLFAutomationLibrary.diagnose_settings_widgets()
unreal.log_warning("=== SETTINGS DIAGNOSTICS ===")
for line in result.split("\n")[:100]:
    unreal.log_warning(line)
