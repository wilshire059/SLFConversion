"""
Export NPC Blueprint text to find component configuration.
"""
import unreal

NPC_BLUEPRINTS = [
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseGuide",
    "/Game/SoulslikeFramework/Blueprints/_Characters/Npcs/B_Soulslike_NPC_ShowcaseVendor",
]

def main():
    unreal.log_warning("\n" + "="*70)
    unreal.log_warning("EXPORTING NPC BLUEPRINT TEXT FROM BP_ONLY")
    unreal.log_warning("="*70 + "\n")
    
    for bp_path in NPC_BLUEPRINTS:
        bp_name = bp_path.split("/")[-1]
        unreal.log_warning(f"\n=== {bp_name} ===")
        
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        if not bp:
            unreal.log_warning(f"  [FAIL] Could not load")
            continue
        
        # Export to text
        export_text = unreal.EditorAssetLibrary.export_text(bp_path)
        if export_text:
            # Look for key lines related to InteractionManager, Name, DialogAsset
            lines = export_text.split('\n')
            for i, line in enumerate(lines):
                if 'AC_AI_InteractionManager' in line or 'DialogAsset' in line or 'VendorAsset' in line:
                    unreal.log_warning(f"  Line {i}: {line[:200]}")
                elif 'Name=' in line and ('Interaction' in lines[max(0,i-5):i+1].__str__() if i > 0 else False):
                    unreal.log_warning(f"  Line {i}: {line[:200]}")

if __name__ == "__main__":
    main()
