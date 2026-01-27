"""
Compare IK setup between SLFConversion and bp_only AnimBP assets.
Exports asset text and identifies differences in IK-related nodes.
"""

import unreal
import re
import json
import os

def export_asset_text(asset_path, output_filename):
    """Export asset to text format for analysis using Exporter."""
    asset = unreal.load_asset(asset_path)
    if not asset:
        print(f"ERROR: Could not load asset: {asset_path}")
        return None

    # Use AssetExporterTask for text export
    export_task = unreal.AssetExportTask()
    export_task.object = asset
    export_task.exporter = None  # Auto-select
    export_task.filename = output_filename
    export_task.selected = False
    export_task.prompt = False
    export_task.automated = True
    export_task.replace_identical = True

    success = unreal.Exporter.run_asset_export_task(export_task)
    if success:
        try:
            with open(output_filename, 'r', encoding='utf-8', errors='ignore') as f:
                return f.read()
        except Exception as e:
            print(f"Error reading exported file: {e}")

    return None

def search_text_for_patterns(text, source_name):
    """Search text for IK-related patterns."""
    if not text:
        return {}

    results = {
        "source": source_name,
        "ik_weight_refs": [],
        "control_rig_refs": [],
        "finger_refs": [],
        "hand_refs": [],
        "input_mapping_refs": [],
        "alpha_refs": [],
        "linked_layer_refs": []
    }

    lines = text.split('\n')

    for i, line in enumerate(lines):
        lower_line = line.lower()

        if 'ikweight' in lower_line or 'ik_weight' in lower_line or 'ik weight' in lower_line:
            results["ik_weight_refs"].append({
                "line": i + 1,
                "content": line.strip()[:200]
            })

        if 'controlrig' in lower_line or 'cr_' in lower_line:
            results["control_rig_refs"].append({
                "line": i + 1,
                "content": line.strip()[:200]
            })

        if 'finger' in lower_line:
            results["finger_refs"].append({
                "line": i + 1,
                "content": line.strip()[:200]
            })

        if 'hand' in lower_line and ('ik' in lower_line or 'ctrl' in lower_line or 'bone' in lower_line):
            results["hand_refs"].append({
                "line": i + 1,
                "content": line.strip()[:200]
            })

        if 'inputmapping' in lower_line or 'input_mapping' in lower_line:
            results["input_mapping_refs"].append({
                "line": i + 1,
                "content": line.strip()[:200]
            })

        if 'alpha' in lower_line and ('input' in lower_line or 'value' in lower_line or 'weight' in lower_line):
            results["alpha_refs"].append({
                "line": i + 1,
                "content": line.strip()[:200]
            })

        if 'linkedanim' in lower_line or 'animlayer' in lower_line:
            results["linked_layer_refs"].append({
                "line": i + 1,
                "content": line.strip()[:200]
            })

    # Summary
    results["summary"] = {
        "ik_weight_count": len(results["ik_weight_refs"]),
        "control_rig_count": len(results["control_rig_refs"]),
        "finger_count": len(results["finger_refs"]),
        "hand_count": len(results["hand_refs"]),
        "input_mapping_count": len(results["input_mapping_refs"]),
        "alpha_count": len(results["alpha_refs"]),
        "linked_layer_count": len(results["linked_layer_refs"])
    }

    return results

def main():
    print("=" * 80)
    print("IK SETUP ANALYSIS: SLFConversion")
    print("=" * 80)

    # Paths
    slf_animbp = "/Game/SoulslikeFramework/Demo/_Animations/Locomotion/AnimBP/ABP_SoulslikeCharacter_Additive"
    slf_control_rig = "/Game/SoulslikeFramework/Demo/SKM/Mannequins/Meshes/CR_SoulslikeFramework"

    # Get AnimBP info
    print("\n[1] Loading AnimBP...")
    asset = unreal.load_asset(slf_animbp)
    if asset:
        print(f"    Class: {asset.get_class()}")
        gen_class = asset.generated_class()
        if gen_class:
            print(f"    Generated Class: {gen_class}")

    # Export AnimBP text
    print("\n[2] Exporting AnimBP to text...")
    output_path = "C:/scripts/SLFConversion/migration_cache/animbp_export_slfconversion.txt"
    slf_text = export_asset_text(slf_animbp, output_path)

    if slf_text:
        print(f"    Exported {len(slf_text)} characters")
        print(f"    Saved to: {output_path}")

        # Search for patterns
        print("\n[3] Searching for IK patterns...")
        search_results = search_text_for_patterns(slf_text, "SLFConversion")

        print(f"\n    Summary:")
        for key, value in search_results.get("summary", {}).items():
            print(f"      {key}: {value}")

        # Print findings
        if search_results.get("ik_weight_refs"):
            print("\n[4] IK Weight References:")
            for ref in search_results["ik_weight_refs"][:30]:
                print(f"    Line {ref['line']}: {ref['content']}")

        if search_results.get("input_mapping_refs"):
            print("\n[5] Input Mapping References:")
            for ref in search_results["input_mapping_refs"][:30]:
                print(f"    Line {ref['line']}: {ref['content']}")

        if search_results.get("alpha_refs"):
            print("\n[6] Alpha/Weight Input References:")
            for ref in search_results["alpha_refs"][:30]:
                print(f"    Line {ref['line']}: {ref['content']}")

        if search_results.get("finger_refs"):
            print("\n[7] Finger References:")
            for ref in search_results["finger_refs"][:30]:
                print(f"    Line {ref['line']}: {ref['content']}")

        if search_results.get("control_rig_refs"):
            print("\n[8] Control Rig References (first 50):")
            for ref in search_results["control_rig_refs"][:50]:
                print(f"    Line {ref['line']}: {ref['content']}")

        if search_results.get("linked_layer_refs"):
            print("\n[9] Linked Anim Layer References:")
            for ref in search_results["linked_layer_refs"][:30]:
                print(f"    Line {ref['line']}: {ref['content']}")

        # Save analysis
        analysis_path = "C:/scripts/SLFConversion/migration_cache/ik_analysis_slfconversion.json"
        with open(analysis_path, 'w', encoding='utf-8') as f:
            json.dump({
                "search_results": search_results
            }, f, indent=2, default=str)
        print(f"\n    Saved analysis to: {analysis_path}")

    else:
        print("    ERROR: Could not export AnimBP text")

    # Export Control Rig
    print("\n[10] Exporting Control Rig to text...")
    cr_output_path = "C:/scripts/SLFConversion/migration_cache/controlrig_export_slfconversion.txt"
    cr_text = export_asset_text(slf_control_rig, cr_output_path)
    if cr_text:
        print(f"    Exported {len(cr_text)} characters")
        print(f"    Saved to: {cr_output_path}")

        # Search CR for IK patterns
        cr_search = search_text_for_patterns(cr_text, "SLFConversion_CR")
        print(f"\n    Control Rig IK Summary:")
        for key, value in cr_search.get("summary", {}).items():
            print(f"      {key}: {value}")

        if cr_search.get("finger_refs"):
            print("\n[11] Control Rig Finger References:")
            for ref in cr_search["finger_refs"][:50]:
                print(f"    Line {ref['line']}: {ref['content']}")

    print("\n" + "=" * 80)
    print("DONE - Now run same script on bp_only project")
    print("=" * 80)

if __name__ == "__main__":
    main()
