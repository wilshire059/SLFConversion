"""
Run asset validation to compare dialog assets between bp_only and SLFConversion
"""
import unreal

# Run export on SLFConversion project
output_folder = "C:/scripts/SLFConversion/migration_cache/dialog_exports_slfconversion"
count = unreal.SLFAssetValidator.export_all_dialog_assets(output_folder)
print(f"[SLFConversion] Exported {count} dialog assets to: {output_folder}")
