import unreal

def log(msg):
    print(f"[DiagBossABP] {msg}")
    unreal.log_warning(f"[DiagBossABP] {msg}")

# Check bp_only version for comparison
bp_only_path = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeBossNew"

log("=" * 70)
log("CHECKING ABP_SoulslikeBossNew STRUCTURE")
log("=" * 70)

abp = unreal.EditorAssetLibrary.load_asset(bp_only_path)
if not abp:
    log("ERROR: Could not load ABP")
else:
    log(f"Loaded: {abp.get_name()}")
    
    # Export to text to see internal structure
    try:
        export_result = abp.export_text()
        
        # Look for PoiseBreakAsset references
        lines = export_result.split('\n')
        poise_lines = []
        for i, line in enumerate(lines):
            if 'PoiseBreakAsset' in line or 'Poise Break Asset' in line:
                context_start = max(0, i-2)
                context_end = min(len(lines), i+3)
                poise_lines.append(f"Line {i}: {line.strip()}")
                
        if poise_lines:
            log(f"Found {len(poise_lines)} references to PoiseBreakAsset:")
            for pl in poise_lines[:20]:  # Limit output
                log(f"  {pl}")
        else:
            log("No PoiseBreakAsset references found in export")
            
        # Look for property bindings
        binding_lines = []
        for i, line in enumerate(lines):
            if 'PropertyBinding' in line or 'Access' in line.lower() and 'property' in line.lower():
                binding_lines.append(f"Line {i}: {line.strip()[:100]}")
                
        if binding_lines:
            log(f"Found {len(binding_lines)} property binding references:")
            for bl in binding_lines[:10]:
                log(f"  {bl}")
                
    except Exception as e:
        log(f"Export error: {e}")
        
log("")
log("=" * 70)
log("DONE")
log("=" * 70)
